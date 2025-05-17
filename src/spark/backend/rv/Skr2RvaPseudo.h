#pragma once
#include <vector>
#include "../../common/LinearAllocator.h"
#include "../../skr/instr/everything.h"
#include "instr/everything.h"

class Skr2RvaPseudo {
public:
    Skr2RvaPseudo(LinearAllocator& allocator, const std::vector<SkrInstruction*>& skrs, std::vector<RvaInstruction*>& out) 
        : allocator(allocator)
        , skrs(skrs)
        , out(out) {  }

    void emit() {
        for (const auto* skr : skrs) {
            auto type = skr->getType();
            if (type == SkrInstruction::Type::Binary) {
                emitBinary((SkrBinary*) skr);
            }
            else {
                printf("Unknown skr type: %d", type);
                std::abort();
            }
        }
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
    const std::vector<SkrInstruction*>& skrs;
    std::vector<RvaInstruction*>& out;
};
