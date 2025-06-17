#pragma once
#include <vector>
#include "../../../common/cfg/CfGraph.h"
#include "ReachingCopiesAnalysis.h"

class CopyPropagation {
public:
    CopyPropagation(CfGraph<SkrInstruction*>* graph, Allocator& allocator)
        : graph(graph)
        , allocator(allocator) {  }


    void run() {
        ReachingCopiesAnalysis rca(graph);
        rca.run();

        const auto blocksCount = graph->getBlocks().size();
        for (size_t i = 1; i < blocksCount - 1; i++) {
            auto* block = getBlock(i);
            auto* annotated = rca.getAnnotated(i);
            
            auto& body = block->getBody();
            const auto instrCount = body.size();
            for (size_t instrIdx = 0; instrIdx < instrCount; instrIdx++) {
                body[instrIdx] = rewriteInstruction(block, annotated, instrIdx);
                if (shouldBeRemoved(body[instrIdx])) {
                    body[instrIdx] = NOP;
                }
            }
            removeNops(body);
        }
    }

private:
    SkrInstruction* rewriteInstruction(CfgBlock<SkrInstruction*>* block, RCABlock* annotated, size_t instrIdx) {
        auto* instr = block->getBody()[instrIdx];
        const auto& copies = annotated->instrCopies[instrIdx];
        if (instr->kind == SkrInstruction::Kind::Binary) {
            auto* it = (SkrBinary*) instr;
            auto* left = replace(it->getLeft(), copies);
            auto* right = replace(it->getRight(), copies);
            if (left != it->getLeft() || right != it->getRight()) {
                return allocator.create<SkrBinary>(it->getDst(), left, it->getOperator(), right);
            }
        }
        else if (instr->kind == SkrInstruction::Kind::Copy) {
            auto* it = (SkrCopy*) instr;
            auto* from = replace(it->getFrom(), copies);
            if (*from == *it->getTo()) {
                return NOP;
            }
            if (from != it->getFrom()) {
                return allocator.create<SkrCopy>(it->getTo(), from);
            }
        }
        else if (instr->kind == SkrInstruction::Kind::FunCall) {
            auto* it = (SkrFunCall*) instr;
            auto& args = it->getArgs();
            for (size_t i = 0; i < args.size(); i++) {
                args.set(i, replace(args[i], copies));
            }
        }

        return instr;
    }

    static SkrValue* replace(SkrValue* value, const std::vector<SkrCopy*>& copies) {
        if (value->isConst()) {
            return value;
        }

        auto* var = value->toSkrVar();
        auto* copy = findCopyByDst(copies, var);
        if (copy != nullptr) {
            return copy->getFrom();
        } else {
            return value;
        }
    }

    static bool shouldBeRemoved(SkrInstruction* skr) {
        if (skr->kind == SkrInstruction::Kind::Copy) {
            auto* it = (SkrCopy*) skr;
            return *it->getFrom() == *it->getTo();
        }
        return false;
    }

    CfgBlock<SkrInstruction*>* getBlock(int idx) { return graph->getBlock(idx); }
    const CfgBlock<SkrInstruction*>* getBlock(int idx) const { return graph->getBlock(idx); }

    static void removeNops(std::vector<SkrInstruction*>& skrs) {
        size_t offset = 0;
        for (size_t i = 0; i < skrs.size() - offset; i++) {
            if (skrs[i] == NOP) {
                offset++;
            }
            skrs[i] = skrs[i + offset];
        }

        skrs.resize(skrs.size() - offset);
    }

    static SkrCopy* findCopyByDst(const std::vector<SkrCopy*>& copies, SkrVar* var) {
        for (auto* c : copies) {
            if (*c->getTo() == *var) {
                return c;
            }
        }
        return nullptr;
    }

    static constexpr SkrInstruction* NOP = nullptr;

    CfGraph<SkrInstruction*>* graph;
    Allocator& allocator;
};
