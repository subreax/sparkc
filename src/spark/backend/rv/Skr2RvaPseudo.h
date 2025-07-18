#pragma once
#include <vector>
#include "../../common/alloc/Allocator.h"
#include "../../common/Error.h"
#include "../../skr/instr/everything.h"
#include "../../skr/SkrFunction.h"
#include "../../symbol/SymbolTable.h"
#include "../../size/SymbolSize.h"
#include "instr/everything.h"
#include "StackFrame.h"
#include "FixedUtils.h"

class Skr2RvaPseudo {
public:
    static void emit(SkrFunction* func, Allocator& allocator, IdentifierGen& idGen, SymbolTable& table, SymbolSize& ss, StackFrame& frame, std::vector<RvaInstruction*>& buf) {
        Skr2RvaPseudo(allocator, idGen, table, ss, frame, buf).emit(func);
    }

private:
    Skr2RvaPseudo(Allocator& allocator, IdentifierGen& idGen, SymbolTable& symbolTable, SymbolSize& ss, StackFrame& frame, std::vector<RvaInstruction*>& out) 
        : allocator(allocator)
        , idGen(idGen)
        , symbolTable(symbolTable)
        , symbolSize(ss)
        , frame(frame)
        , out(out)
        , tempReg(allocator.create<RvaRegister>(RvReg::T6)) {  }

    void emit(SkrFunction* func) {
        auto* prologue = allocator.create<RvaPrologue>();
        auto* epilogue = allocator.create<RvaEpilogue>();
        bool shouldSaveRa = hasFunctionCalls(func->getInstructions());
        if (shouldSaveRa) {
            frame.occupy(4);
            prologue->saveRa();
            epilogue->loadRa();
        }

        add<RvaLabel>(func->getName());
        addInstr(prologue);

        auto params = func->getParams();
        for (size_t i = 0; i < params.size(); i++) {
            if (i < 8) {
                auto* to = toPseudo(params[i]);
                auto* from = allocator.create<RvaRegister>(getArgReg(i));
                add<RvaMov>(to, from);
            } else {
                frame.bindParam(params[i]->getId());
            }
        }

        for (const auto* skr : func->getInstructions()) {
            switch (skr->kind) {
            case SkrInstruction::Kind::Binary:
                emitBinary((SkrBinary*) skr);
                break;
            
            case SkrInstruction::Kind::Copy:
                emitCopy((SkrCopy*) skr);
                break;

            case SkrInstruction::Kind::Jump:
                emitJump((SkrJump*) skr);
                break;

            case SkrInstruction::Kind::Label:
                emitLabel((SkrLabel*) skr);
                break;

            case SkrInstruction::Kind::Branch:
                emitBranch((SkrBranch*) skr);
                break;

            case SkrInstruction::Kind::FunCall:
                emitFunCall((SkrFunCall*) skr);
                break;

            case SkrInstruction::Kind::Int2Float:
                emitInt2Float((SkrInt2Float*) skr);
                break;

            case SkrInstruction::Kind::Float2Int:
                emitFloat2Int((SkrFloat2Int*) skr);
                break;

            case SkrInstruction::Kind::Load:
                emitLoad((SkrLoad*) skr);
                break;

            case SkrInstruction::Kind::Store:
                emitStore((SkrStore*) skr);
                break;

            case SkrInstruction::Kind::GetAddr:
                emitGetAddr((SkrGetAddr*) skr);
                break;

            case SkrInstruction::Kind::CopyToOffset:
                emitCopyToOffset((SkrCopyToOffset*) skr);
                break;

            case SkrInstruction::Kind::CopyFromOffset:
                emitCopyFromOffset((SkrCopyFromOffset*) skr);
                break;

            default:
                sparkError("Skr2RvaPseudo", "Unknown skr kind: %d", skr->kind);
            }
        }

        auto* resultPseudoReg = allocator.create<RvaPseudoReg>(func->getRetVar()->getId());
        auto* a0Reg = allocator.create<RvaRegister>(RvReg::A0);
        add<RvaMov>(a0Reg, resultPseudoReg);
        addInstr(epilogue);
        add<RvaRet>();
    }

    void emitBinary(SkrBinary* it) {
        auto dstType = symbolTable.get(it->getDst()->getId())->kind;
        auto op = it->getOperator();
        if (dstType == SymbolType::Kind::Float && op == SkrBinary::Operator::Mul) {
            emitFixedMul(it);
        }
        else if (dstType == SymbolType::Kind::Float && op == SkrBinary::Operator::Div) {
            sparkError("Skr2RvaPseudo", "Fixed division is not implemented");
        }
        else {
            add<RvaBinary>(
                toPseudo(it->getDst()), 
                toPseudo(it->getLeft()), 
                RvaBinary::mapOperator(op), 
                toPseudo(it->getRight())
            );
        }
    }

    void emitFixedMul(SkrBinary* it) {
        auto* dst = toPseudo(it->getDst());
        auto* dstH = tempReg;
        auto* left = toPseudo(it->getLeft());
        auto* right = toPseudo(it->getRight());
        add<RvaBinary>(dst, left, RvaBinary::Operator::Mul, right);
        add<RvaBinary>(dstH, left, RvaBinary::Operator::MulH, right);
        add<RvaBinary>(dst, dst, RvaBinary::Operator::ShiftRight, newImm(15));
        add<RvaBinary>(dstH, dstH, RvaBinary::Operator::ShiftLeft, newImm(17));
        add<RvaBinary>(dst, dstH, RvaBinary::Operator::Or, dst);
    }

    void emitCopy(SkrCopy* it) {
        copy(it->getTo(), 0, it->getFrom(), 0);
    }

    void emitJump(SkrJump* it) {
        add<RvaJump>(it->getLabel());
    }

    void emitLabel(SkrLabel* it) {
        add<RvaLabel>(it->getLabel());
    }

    void emitBranch(SkrBranch* it) {
        add<RvaBranch>(
            toPseudo(it->getLeft()),
            RvaBranch::mapOperator(it->getOperator()),
            toPseudo(it->getRight()),
            it->getLabel()
        );
    }

    void emitFunCall(SkrFunCall* it) {
        const auto& skrArgs = it->getArgs();
        for (size_t i = 0; i < skrArgs.size(); i++) {
            auto* to = getArgDst(i);
            auto* from = toPseudo(skrArgs[i]);
            add<RvaMov>(to, from);
        }

        auto* retVal = toPseudo(it->getRetVar());
        add<RvaCall>(it->getName());
        add<RvaMov>(retVal, allocator.create<RvaRegister>(RvReg::A0));

        frame.popArgs();
    }

    void emitInt2Float(SkrInt2Float* it) {
        add<RvaBinary>(toPseudo(it->getDst()), toPseudo(it->getSrc()), RvaBinary::Operator::ShiftLeft, newImm(15));
    }

    void emitFloat2Int(SkrFloat2Int* it) {
        // todo: could be wrong
        add<RvaBinary>(toPseudo(it->getDst()), toPseudo(it->getSrc()), RvaBinary::Operator::ShiftRight, newImm(15));
    }

    void emitLoad(SkrLoad* it) {
        add<RvaMov>(tempReg, toPseudo(it->getFrom()));
        loadBytes(it->getTo(), tempReg, it->getFromOffset());
    }

    void emitStore(SkrStore* it) {
        add<RvaMov>(tempReg, toPseudo(it->getTo()));
        storeBytes(tempReg, it->getToOffset(), it->getFrom());
    }

    void emitGetAddr(SkrGetAddr* it) {
        add<RvaGetAddress>(toPseudo(it->getTo()), toPseudo(it->getVar()));
    }

    void emitCopyToOffset(SkrCopyToOffset* it) {
        copy(it->getTo(), it->getToOffset(), it->getFrom(), 0);
    }

    void emitCopyFromOffset(SkrCopyFromOffset* it) {
        copy(it->getTo(), 0, it->getFrom(), it->getFromOffset());
    }

    template<typename T, typename... Args>
    inline void add(Args... args) {
        out.emplace_back(allocator.create<T>(args...));
    }

    inline void addInstr(RvaInstruction* instr) {
        out.emplace_back(instr);
    }

    RvaValue* toPseudo(const SkrValue* value, int offsetIfMem = 0) {
        auto kind = value->kind;

        switch (kind) {
        case SkrValue::Kind::Const: {
            auto* it = (const SkrConst*) value;
            auto* constant = it->getConst();
            int32_t val;
            if (constant->isInt()) {
                val = constant->intValue();
            }
            else if (constant->isFloat()) {
                val = FixedUtils::fromFloat(constant->floatValue());
            }
            else {
                sparkError("Skr2RvaPseudo", "Unknown Constant type: %d", constant->type);
            }
            return allocator.create<RvaImm>(val);
        }
        case SkrValue::Kind::Var: {
            auto* it = (const SkrVar*) value;
            if (isStructure(it)) {
                return allocator.create<RvaPseudoMem>(it->getId(), offsetIfMem);
            } else {
                return allocator.create<RvaPseudoReg>(it->getId());
            }
        }
        default:
            sparkError("Skr2RvaPseudo", "Unknown Skrvalue kind: %d", kind);
            return nullptr;
        }
    }

    inline RvaImm* newImm(int32_t val) {
        return allocator.create<RvaImm>(val);
    }

    inline RvaPseudoReg* newPseudo(const char* name) {
        return allocator.create<RvaPseudoReg>(idGen.unique(name));
    }

    inline RvaPseudoMem* newPseudoMem(StringRef id, int offset) {
        return allocator.create<RvaPseudoMem>(id, offset);
    }

    inline RvaMemory* newMemory(RvaRegister* base, int offset) {
        return allocator.create<RvaMemory>(base->getReg(), offset);
    }

    RvaValue* getArgDst(int argIndex) {
        if (argIndex < 8) {
            return allocator.create<RvaRegister>(getArgReg(argIndex));
        } else {
            return frame.pushArg();
        }
    }

    RvReg getArgReg(int idx) {
        if (idx < 8) {
            return (RvReg) ((int) RvReg::A0 + idx);
        }
        sparkError("Skr2RvaPseudo", "getArgReg(idx): idx should be in range [0; 7]");
        return RvReg::ZERO;
    }

    bool hasFunctionCalls(const BoundArray<SkrInstruction*>& skrs) const {
        for (SkrInstruction* it : skrs) {
            if (it->kind == SkrInstruction::Kind::FunCall) {
                return true;
            }
        }
        return false;
    }

    bool isStructure(const SkrValue* val) const {
        return val->isVar() && isStructure(val->toSkrVar()->getId());
    }

    bool isStructure(StringRef id) const {
        return symbolTable.get(id)->kind == SymbolType::Kind::Structure;
    }

    int getSize(SkrValue* val) const {
        if (val->isConst()) {
            return 4;
        }
        else if (val->isVar()) {
            return symbolSize.get(val->toSkrVar()->getId());
        }

        sparkError("Skr2RvaPseudo", "getSize(): Unreachable");
        return 0;
    }

    void copy(SkrValue* to, int toOffset, SkrValue* from, int fromOffset) {
        auto sz = getSize(from) - fromOffset;
        for (size_t off = 0; off < sz; off += 4) {
            add<RvaMov>(toPseudo(to, toOffset + off), toPseudo(from, fromOffset + off));
        }
    }

    void storeBytes(RvaRegister* toAddrReg, int offset, SkrValue* from) {
        size_t sz = getSize(from);
        for (size_t off = 0; off < sz; off += 4) {
            add<RvaMov>(newMemory(toAddrReg, offset + off), toPseudo(from, off));
        }
    }

    void loadBytes(SkrValue* to, RvaRegister* fromAddrReg, int offset) {
        size_t sz = getSize(to);
        for (size_t off = 0; off < sz; off += 4) {
            add<RvaMov>(toPseudo(to, off), newMemory(fromAddrReg, offset + off));
        }
    }


    Allocator& allocator;
    IdentifierGen& idGen;
    SymbolTable& symbolTable;
    SymbolSize& symbolSize;
    StackFrame& frame;
    std::vector<RvaInstruction*>& out;
    RvaRegister* tempReg;
};
