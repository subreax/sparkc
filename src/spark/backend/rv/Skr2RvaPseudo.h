#pragma once
#include <vector>
#include "../../common/alloc/Allocator.h"
#include "../../common/Error.h"
#include "../../skr/instr/everything.h"
#include "../../skr/SkrFunction.h"
#include "instr/everything.h"
#include "StackFrame.h"
#include "FixedUtils.h"

class Skr2RvaPseudo {
public:
    static void emit(SkrFunction* func, Allocator& allocator, IdentifierGen& idGen, SymbolTable& table, StackFrame& frame, std::vector<RvaInstruction*>& buf) {
        Skr2RvaPseudo(allocator, idGen, table, frame, buf).emit(func);
    }

private:
    Skr2RvaPseudo(Allocator& allocator, IdentifierGen& idGen, SymbolTable& symbolTable, StackFrame& frame, std::vector<RvaInstruction*>& out) 
        : allocator(allocator)
        , idGen(idGen)
        , table(symbolTable)
        , frame(frame)
        , out(out) {  }

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
                auto* to = frame.getOrPush(params[i]->getId());
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
        auto dstType = table.get(it->getDst()->getId())->kind;
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
        auto* dstH = newPseudo("tmp");
        auto* left = toPseudo(it->getLeft());
        auto* right = toPseudo(it->getRight());
        add<RvaBinary>(dst, left, RvaBinary::Operator::Mul, right);
        add<RvaBinary>(dstH, left, RvaBinary::Operator::MulH, right);
        add<RvaBinary>(dst, dst, RvaBinary::Operator::ShiftRight, newImm(15));
        add<RvaBinary>(dstH, dstH, RvaBinary::Operator::ShiftLeft, newImm(17));
        add<RvaBinary>(dst, dstH, RvaBinary::Operator::Or, dst);
    }

    void emitCopy(SkrCopy* it) {
        add<RvaMov>(toPseudo(it->getTo()), toPseudo(it->getFrom()));
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

    template<typename T, typename... Args>
    inline void add(Args... args) {
        out.emplace_back(allocator.create<T>(args...));
    }

    inline void addInstr(RvaInstruction* instr) {
        out.emplace_back(instr);
    }

    inline RvaValue* toPseudo(const SkrValue* value) {
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
            return allocator.create<RvaPseudoReg>(it->getId());
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

    Allocator& allocator;
    IdentifierGen& idGen;
    SymbolTable& table;
    StackFrame& frame;
    std::vector<RvaInstruction*>& out;
};
