#pragma once
#include "FixedUtils.h"
#include "StackFrame.h"
#include "sparkc/backend/rv/instr/everything.h"
#include "sparkc/common/Error.h"
#include "sparkc/common/alloc/Allocator.h"
#include "sparkc/size/SymbolSize.h"
#include "sparkc/skr/SkrFunction.h"
#include "sparkc/skr/instr/everything.h"
#include "sparkc/symbol/SymbolTable.h"
#include <unordered_set>
#include <vector>

class Skr2RvaPseudo {
public:
    static void emit(
        SkrFunction* func,
        Allocator& allocator,
        IdentifierGen& idGen,
        SymbolTable& table,
        SymbolSize& ss,
        StackFrame& frame,
        std::vector<RvaInstruction*>& buf
    ) {
        Skr2RvaPseudo(allocator, idGen, table, ss, frame, buf).emit(func);
    }

private:
    Skr2RvaPseudo(
        Allocator& allocator,
        IdentifierGen& idGen,
        SymbolTable& symbolTable,
        SymbolSize& ss,
        StackFrame& frame,
        std::vector<RvaInstruction*>& out
    )
        : allocator(allocator)
        , idGen(idGen)
        , symbolTable(symbolTable)
        , symbolSize(ss)
        , frame(frame)
        , out(out)
        , tempReg(newRegister(RvReg::T6)) { }

    void emit(SkrFunction* func) {
        _retInMem = getSize(func->getRetVar()) > 8;

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

        saveParams(func);

        for (const auto* skr : func->getInstructions()) {
            switch (skr->kind) {
            case SkrInstruction::Kind::Binary: emitBinary((SkrBinary*) skr); break;
            case SkrInstruction::Kind::Copy: emitCopy((SkrCopy*) skr); break;
            case SkrInstruction::Kind::Jump: emitJump((SkrJump*) skr); break;
            case SkrInstruction::Kind::Label: emitLabel((SkrLabel*) skr); break;
            case SkrInstruction::Kind::Branch: emitBranch((SkrBranch*) skr); break;
            case SkrInstruction::Kind::FunCall: emitFunCall((SkrFunCall*) skr); break;
            case SkrInstruction::Kind::Int2Float: emitInt2Float((SkrInt2Float*) skr); break;
            case SkrInstruction::Kind::Float2Int: emitFloat2Int((SkrFloat2Int*) skr); break;
            case SkrInstruction::Kind::Load: emitLoad((SkrLoad*) skr); break;
            case SkrInstruction::Kind::Store: emitStore((SkrStore*) skr); break;
            case SkrInstruction::Kind::GetAddr: emitGetAddr((SkrGetAddr*) skr); break;
            case SkrInstruction::Kind::CopyToOffset: emitCopyToOffset((SkrCopyToOffset*) skr); break;
            case SkrInstruction::Kind::CopyFromOffset: emitCopyFromOffset((SkrCopyFromOffset*) skr); break;

            default:
                sparkError("Skr2RvaPseudo", "Unknown skr kind: %d", skr->kind);
            }
        }

        if (!_retInMem) {
            auto* resultPseudoReg = toPseudo(func->getRetVar());
            auto* a0Reg = getArgReg(0);
            add<RvaMov>(a0Reg, resultPseudoReg);
        }

        addInstr(epilogue);
        add<RvaRet>();
    }

    void emitBinary(SkrBinary* it) {
        auto dstType = symbolTable.get(it->getDst()->getId())->kind;
        auto op = it->getOperator();
        if (dstType == SymbolType::Kind::Float && op == SkrBinary::Operator::Mul) {
            add<RvaBinary>(
                toPseudo(it->getDst()),
                toPseudo(it->getLeft()),
                RvaBinary::Operator::_FixedMul,
                toPseudo(it->getRight())
            );
        }
        else if (dstType == SymbolType::Kind::Float && op == SkrBinary::Operator::Div) {
            add<RvaBinary>(
                toPseudo(it->getDst()),
                toPseudo(it->getLeft()),
                RvaBinary::Operator::_FixedDiv,
                toPseudo(it->getRight())
            );
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

    void emitFunCall(SkrFunCall* func) {
        bool retInMem = getSize(func->getRetVar()) > 8;

        const auto& skrArgs = func->getArgs();
        int argIdx = 0;
        RvaValue* retVar = toPseudo(func->getRetVar());
        if (retInMem) {
            if (isReplacedToPtr(func->getRetVar())) {
                add<RvaMov>(getArgReg(0), retVar);
            }
            else {
                add<RvaReserveOnStack>(retVar);
                add<RvaGetAddress>(getArgReg(0), retVar);
            }
            argIdx++;
        }

        add<RvaBeginTempStack>();
        for (auto* arg : skrArgs) {
            size_t argSz = getSize(arg);
            if (argSz > 8) {
                // placeArgOnStack(arg, argSz, argIdx);
                passArgByRef(arg, argIdx);
                argIdx++;
            }
            else if (argSz > 4) {
                // todo
                sparkError("Skr2RvaPseudo", "Not implemented");
            }
            else {
                add<RvaMov>(getArgDst(argIdx), toPseudo(arg));
                argIdx++;
            }
        }
        add<RvaEndTempStack>();
        add<RvaCall>(func->getName());

        if (!retInMem) {
            add<RvaMov>(retVar, getArgReg(0)); // todo: support 4-8 bytes
        }
    }

    void placeArgOnStack(const SkrValue* skrArg, size_t sz, int regIdx) {
        StringRef argId = idGen.unique("arg");
        symbolTable.declare(argId, getType(skrArg));
        SkrVar* arg = allocator.create<SkrVar>(argId);

        copy(arg, 0, skrArg, 0);
        add<RvaGetAddress>(getArgDst(regIdx), toPseudo(arg));
    }

    void passArgByRef(const SkrValue* skrArg, int regIdx) {
        add<RvaGetAddress>(getArgDst(regIdx), toPseudo(skrArg));
    }

    void emitInt2Float(SkrInt2Float* it) {
        add<RvaBinary>(
            toPseudo(it->getDst()),
            toPseudo(it->getSrc()),
            RvaBinary::Operator::ShiftLeft,
            newImm(15)
        );
    }

    void emitFloat2Int(SkrFloat2Int* it) {
        // todo: could be wrong
        add<RvaBinary>(
            toPseudo(it->getDst()),
            toPseudo(it->getSrc()),
            RvaBinary::Operator::ShiftRight,
            newImm(15)
        );
    }

    void emitLoad(SkrLoad* it) {
        loadBytes(it->getTo(), it->getFrom(), it->getFromOffset());
    }

    void emitStore(SkrStore* it) {
        storeBytes(it->getTo(), it->getToOffset(), it->getFrom());
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

    template <typename T, typename... Args>
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
                sparkError(
                    "Skr2RvaPseudo",
                    "Unknown Constant type: %d",
                    constant->type
                );
            }
            return allocator.create<RvaImm>(val);
        }
        case SkrValue::Kind::Var: {
            auto* it = (const SkrVar*) value;
            if (!isReplacedToPtr(it) && isStructure(it)) {
                return allocator.create<RvaPseudoMem>(it->getId(), offsetIfMem);
            }
            else {
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

    inline RvaRegister* newRegister(RvReg reg) {
        return RvaRegister::get(reg);
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

    inline RvaMemory* newMemory(RvReg base, int offset) {
        return allocator.create<RvaMemory>(base, offset);
    }

    RvaValue* getArgDst(int argIndex) {
        if (argIndex < 8) {
            return getArgReg(argIndex);
        }
        else {
            return frame.pushArg();
        }
    }

    RvaValue* getParam(int argIndex) {
        if (argIndex < 8) {
            return getArgReg(argIndex);
        }
        else {
            return newMemory(RvReg::S0, (argIndex - 8) * 4);
        }
    }

    RvaRegister* getArgReg(int idx) {
        if (idx < 8) {
            return newRegister((RvReg) ((int) RvReg::A0 + idx));
        }
        sparkError("Skr2RvaPseudo", "getArgReg(idx): idx should be in range [0; 7]");
        return newRegister(RvReg::ZERO);
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

    int getSize(const SkrValue* val) const {
        if (val->isConst()) {
            return 4;
        }
        else if (val->isVar()) {
            return symbolSize.get(val->toSkrVar()->getId());
        }

        sparkError("Skr2RvaPseudo", "getSize(): Unreachable");
        return 0;
    }

    void copy(const SkrValue* to, int toOffset, const SkrValue* from, int fromOffset) {
        bool isToReplacedToPtr = isReplacedToPtr(to);
        bool isFromReplacedToPtr = isReplacedToPtr(from);
        if (isToReplacedToPtr && isFromReplacedToPtr) {
            copyBytesFromPtrToPtr(to, toOffset, from, fromOffset);
        }
        else if (isToReplacedToPtr) {
            if (fromOffset != 0) {
                sparkError("Skr2RvaPseudo", "Store value from offset is not supported");
            }
            storeBytes(to, toOffset, from);
        }
        else if (isFromReplacedToPtr) {
            if (toOffset != 0) {
                sparkError("Skr2RvaPseudo", "Load value to offset is not supported");
            }
            loadBytes(to, from, fromOffset);
        }
        else {
            auto sz = getSize(from) - fromOffset;
            for (size_t off = 0; off < sz; off += 4) {
                add<RvaMov>(
                    toPseudo(to, toOffset + off),
                    toPseudo(from, fromOffset + off)
                );
            }
        }
    }

    void copyBytesFromPtrToPtr(
        const SkrValue* to,
        int toOffset,
        const SkrValue* from,
        int fromOffset
    ) {
        add<RvaMov>(tempReg, toPseudo(to));
        auto* tempReg2 = newRegister(RvReg::T1);
        add<RvaMov>(tempReg2, toPseudo(from));
        size_t sz = getSize(from);
        for (size_t off = 0; off < sz; off += 4) {
            add<RvaMov>(
                newMemory(tempReg, toOffset + off),
                newMemory(tempReg2, fromOffset + off)
            );
        }
    }

    void storeBytes(const SkrValue* to, int offset, const SkrValue* from) {
        add<RvaMov>(tempReg, toPseudo(to));
        size_t sz = getSize(from);
        for (size_t off = 0; off < sz; off += 4) {
            add<RvaMov>(
                newMemory(tempReg, offset + off),
                toPseudo(from, off)
            );
        }
    }

    void loadBytes(const SkrValue* to, const SkrValue* from, int offset) {
        add<RvaMov>(tempReg, toPseudo(from));
        size_t sz = getSize(to);
        for (size_t off = 0; off < sz; off += 4) {
            add<RvaMov>(
                toPseudo(to, off),
                newMemory(tempReg, offset + off)
            );
        }
    }

    void saveParams(SkrFunction* func) {
        int argRegIdx = 0;
        if (_retInMem) {
            setReplacedToPtr(func->getRetVar());
            add<RvaMov>(
                toPseudo(func->getRetVar()),
                getArgReg(0)
            );
            argRegIdx++;
        }

        auto params = func->getParams();
        for (auto* param : params) {
            if (getSize(param) > 8) {
                setReplacedToPtr(param);
            }

            // todo: handle 2 regs
            add<RvaMov>(
                toPseudo(param),
                getParam(argRegIdx)
            );
            argRegIdx++;
        }
    }

    void setReplacedToPtr(const SkrVar* var) {
        replacedToPtr.emplace(var->getId());
    }

    bool isReplacedToPtr(const SkrValue* val) {
        return val->isVar() && isReplacedToPtr(val->toSkrVar());
    }

    bool isReplacedToPtr(const SkrVar* var) {
        return replacedToPtr.find(var->getId()) != replacedToPtr.end();
    }

    SymbolType* getType(const SkrValue* val) {
        if (val->isConst()) {
            return val->toSkrConst()->getConst()->type;
        }
        else if (val->isVar()) {
            return symbolTable.get(val->toSkrVar()->getId());
        }
        sparkError("Skr2RvaPseudo", "Unknown SkrVar kind: %d", val->kind);
        return nullptr;
    }

    Allocator& allocator;
    IdentifierGen& idGen;
    SymbolTable& symbolTable;
    SymbolSize& symbolSize;
    StackFrame& frame;
    std::vector<RvaInstruction*>& out;
    RvaRegister* tempReg;
    bool _retInMem;

    std::unordered_set<StringRef> replacedToPtr;
};
