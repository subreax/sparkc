#pragma once
#include <vector>
#include "../../common/LinearAllocator.h"
#include "../../skr/instr/everything.h"
#include "../../skr/SkrFunction.h"
#include "instr/everything.h"
#include "StackFrame.h"

class Skr2RvaPseudo {
public:
    static void emit(LinearAllocator& allocator, StackFrame& frame, SkrFunction* func, std::vector<RvaInstruction*>& out) {
        Skr2RvaPseudo(allocator, frame, out).emit(func);
    }

    Skr2RvaPseudo(LinearAllocator& allocator, StackFrame& frame, std::vector<RvaInstruction*>& out) 
        : allocator(allocator)
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

        add(allocator.create<RvaLabel>(func->getName()));
        add(prologue);

        auto params = func->getParams();
        for (size_t i = 0; i < params.size(); i++) {
            if (i < 8) {
                auto* to = frame.getOrPush(params[i]->getId());
                auto* from = allocator.create<RvaRegister>(getArgReg(i));
                add(allocator.create<RvaMov>(to, from));
            } else {
                frame.bindParam(params[i]->getId());
            }
        }

        for (const auto* skr : func->getInstructions()) {
            auto type = skr->getType();
            switch (type) {
            case SkrInstruction::Type::Binary:
                emitBinary((SkrBinary*) skr);
                break;
            
            case SkrInstruction::Type::Copy:
                emitCopy((SkrCopy*) skr);
                break;

            case SkrInstruction::Type::Jump:
                emitJump((SkrJump*) skr);
                break;

            case SkrInstruction::Type::Label:
                emitLabel((SkrLabel*) skr);
                break;

            case SkrInstruction::Type::Branch:
                emitBranch((SkrBranch*) skr);
                break;

            case SkrInstruction::Type::FunCall:
                emitFunCall((SkrFunCall*) skr);
                break;

            default:
                printf("Unknown skr type: %d", type);
                std::abort();
            }
        }

        auto* resultPseudoReg = allocator.create<RvaPseudoReg>(func->getResultIdentifier());
        auto* a0Reg = allocator.create<RvaRegister>(RvReg::A0);
        add(allocator.create<RvaMov>(a0Reg, resultPseudoReg));
        add(epilogue);
        add(allocator.create<RvaRet>());
    }

private:
    void emitBinary(SkrBinary* it) {
        auto* instr = allocator.create<RvaBinary>(
            toPseudo(it->getDst()),
            toPseudo(it->getLeft()),
            RvaBinary::mapOperator(it->getOperator()),
            toPseudo(it->getRight())
        );
        add(instr);
    }

    void emitCopy(SkrCopy* it) {
        add(allocator.create<RvaMov>(
            toPseudo(it->getTo()),
            toPseudo(it->getFrom())
        ));
    }

    void emitJump(SkrJump* it) {
        add(allocator.create<RvaJump>(it->getLabel()));
    }

    void emitLabel(SkrLabel* it) {
        add(allocator.create<RvaLabel>(it->getLabel()));
    }

    void emitBranch(SkrBranch* it) {
        add(allocator.create<RvaBranch>(
            toPseudo(it->getLeft()),
            RvaBranch::mapOperator(it->getOperator()),
            toPseudo(it->getRight()),
            it->getLabel()
        ));
    }

    void emitFunCall(SkrFunCall* it) {
        const auto& skrArgs = it->getArgs();
        for (size_t i = 0; i < skrArgs.size(); i++) {
            auto* to = getArgDst(i);
            auto* from = toPseudo(skrArgs[i]);
            out.emplace_back(allocator.create<RvaMov>(to, from));
        }

        auto* retVal = toPseudo(it->getRetVar());
        out.emplace_back(allocator.create<RvaCall>(it->getName()));
        out.emplace_back(allocator.create<RvaMov>(retVal, allocator.create<RvaRegister>(RvReg::A0)));

        frame.popArgs();
    }

    inline void add(RvaInstruction* instr) {
        out.emplace_back(instr);
    }

    inline RvaValue* toPseudo(const SkrValue* value) {
        auto type = value->getType();

        switch (type) {
        case SkrValue::Type::Const: {
            auto* it = (const SkrConst*) value;
            return allocator.create<RvaImm>(it->getConst());
        }
        case SkrValue::Type::Var: {
            auto* it = (const SkrVar*) value;
            return allocator.create<RvaPseudoReg>(it->getId());
        }
        default:
            printf("Unknown skr value type: %d", type);
            std::abort();
        }
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
        printf("Arg reg should be in range [0; 7]\n");
        std::abort();
        return RvReg::ZERO;
    }

    bool hasFunctionCalls(const std::vector<SkrInstruction*>& skrs) const {
        for (SkrInstruction* it : skrs) {
            if (it->getType() == SkrInstruction::Type::FunCall) {
                return true;
            }
        }
        return false;
    }

    LinearAllocator& allocator;
    StackFrame& frame;
    std::vector<RvaInstruction*>& out;
};
