#pragma once
#include <vector>
#include "../../common/LinearAllocator.h"
#include "../../skr/instr/everything.h"
#include "../../skr/SkrFunction.h"
#include "instr/everything.h"

class Skr2RvaPseudo {
public:
    static void emit(LinearAllocator& allocator, SkrFunction* func, std::vector<RvaInstruction*>& out) {
        Skr2RvaPseudo(allocator, func, out).emit();
    }

    Skr2RvaPseudo(LinearAllocator& allocator, SkrFunction* func, std::vector<RvaInstruction*>& out) 
        : allocator(allocator)
        , func(func)
        , out(out) {  }

    void emit() {
        add(allocator.create<RvaLabel>(func->getName()));
        add(allocator.create<RvaPrologue>(0));
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

            default:
                printf("Unknown skr type: %d", type);
                std::abort();
            }
        }

        auto* resultPseudoReg = allocator.create<RvaPseudoReg>(func->getResultIdentifier());
        auto* a0Reg = allocator.create<RvaRegister>(RvReg::A0);
        add(allocator.create<RvaMov>(a0Reg, resultPseudoReg));
        add(allocator.create<RvaEpilogue>(0));
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

    inline void add(RvaInstruction* instr) {
        out.emplace_back(instr);
    }

    inline RvaValue* toPseudo(SkrValue* value) {
        auto type = value->getType();

        switch (type) {
        case SkrValue::Type::Const: {
            auto* it = (SkrConst*) value;
            return allocator.create<RvaImm>(it->getConst());
        }
        case SkrValue::Type::Var: {
            auto* it = (SkrVar*) value;
            return allocator.create<RvaPseudoReg>(it->getId());
        }
        default:
            printf("Unknown skr value type: %d", type);
            std::abort();
        }
    }

    LinearAllocator& allocator;
    SkrFunction* func;
    std::vector<RvaInstruction*>& out;
};
