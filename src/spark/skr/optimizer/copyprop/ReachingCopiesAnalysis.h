#pragma once
#include "ReachingCopies.h"
#include "../../instr/everything.h"
#include "../../../common/cfg/CfGraph.h"
#include "RCABlock.h"
#include "Uniqueue.h"


class ReachingCopiesAnalysis {
public:
    ReachingCopiesAnalysis(CfGraph<SkrInstruction*>* graph)
        : graph(graph)
        , annotatedBlocks(graph->getBlocks().size()) {  }

    void run() {
        std::vector<SkrCopy*> allCopies;
        getAllCopies(allCopies);

        Uniqueue<size_t> pendingBlocks;

        for (size_t i = 1; i < annotatedBlocks.size() - 1; i++) {
            auto* ab = getAnnotated(i);
            ab->blockCopies = allCopies;
            pendingBlocks.add(i);
        }

        std::vector<SkrCopy*> incomingCopies;
        while (pendingBlocks.isNotEmpty()) {
            size_t blockIdx = pendingBlocks.peek();
            pendingBlocks.pop();
            RCABlock* annotation = getAnnotated(blockIdx);
            incomingCopies.clear();

            RCABlock oldAnnotation = *annotation;
            meet(blockIdx, allCopies, incomingCopies);
            transfer(blockIdx, incomingCopies);
            if (*annotation != oldAnnotation) {
                addAllSuccessors(blockIdx, pendingBlocks);
            }
        }
    }

    RCABlock* getAnnotated(size_t idx) { return &annotatedBlocks[idx]; }
    const RCABlock* getAnnotated(size_t idx) const { return &annotatedBlocks[idx]; }

    const std::vector<RCABlock>& getResult() const { return annotatedBlocks; }

private:
    void meet(size_t blockIdx, const std::vector<SkrCopy*>& allCopies, std::vector<SkrCopy*>& out) {
        out = allCopies;
        auto it = graph->precedessorsIterator(blockIdx);
        auto end = graph->pEnd();
        while (it != end) {
            CfgBlock<SkrInstruction*>* predecessor = *it;
            size_t predIdx = predecessor->getIdx();
            ReachingCopiesUtils::intersect(out, getAnnotated(predIdx)->blockCopies);
            ++it;
        }
    }

    void transfer(size_t blockIdx, const std::vector<SkrCopy*>& incomingCopies) {
        ReachingCopies currentCopies(incomingCopies);
        auto* block = graph->getBlock(blockIdx);
        auto* annotated = getAnnotated(blockIdx);
        annotated->clear();

        for (auto* instr : block->getBody()) {
            annotated->addInstrCopies(currentCopies);

            if (instr->kind == SkrInstruction::Kind::Copy) {
                auto* copyInstr = (SkrCopy*) instr;
                currentCopies.kill(copyInstr->getTo());
                currentCopies.add(copyInstr);
            }
            else if (instr->kind == SkrInstruction::Kind::Binary) {
                auto* binInstr = (SkrBinary*) instr;
                currentCopies.kill(binInstr->getDst());
            }
            else if (instr->kind == SkrInstruction::Kind::FunCall) {
                auto* callInstr = (SkrFunCall*) instr;
                currentCopies.kill(callInstr->getRetVar());
            }
            else if (instr->kind == SkrInstruction::Kind::Float2Int) {
                auto* it = (SkrFloat2Int*) instr;
                currentCopies.kill(it->getDst());
            }
            else if (instr->kind == SkrInstruction::Kind::Int2Float) {
                auto* it = (SkrInt2Float*) instr;
                currentCopies.kill(it->getDst());
            }
            else if (instr->kind == SkrInstruction::Kind::CopyToOffset) {
                auto* it = (SkrCopyToOffset*) instr;
                currentCopies.kill(it->getTo()->toSkrVar());
            }
            else if (instr->kind == SkrInstruction::Kind::CopyFromOffset) {
                auto* it = (SkrCopyFromOffset*) instr;
                currentCopies.kill(it->getTo()->toSkrVar());
            }
            else if (instr->kind == SkrInstruction::Kind::Load) {
                sparkError("ReachingCopiesAnalysis", "Load is not implemented");
            }
            else if (instr->kind == SkrInstruction::Kind::Store) {
                sparkError("ReachingCopiesAnalysis", "Store is not implemented");
            }
        }
        annotated->blockCopies = currentCopies.getCopies();
    }

    void getAllCopies(std::vector<SkrCopy*>& out) {
        const auto& blocks = graph->getBlocks();
        for (auto* block : blocks) {
            getAllCopies(block, out);
        }
    }

    void getAllCopies(CfgBlock<SkrInstruction*>* block, std::vector<SkrCopy*>& out) {
        for (auto* skr : block->getBody()) {
            if (skr->kind == SkrInstruction::Kind::Copy) {
                auto* skrCopy = (SkrCopy*) skr;
                if (!ReachingCopiesUtils::contains(out, skrCopy)) {
                    out.emplace_back(skrCopy);
                }
            }
        }
    }

    void addAllSuccessors(size_t blockIdx, Uniqueue<size_t>& dst) {
        auto it = graph->successorsIterator(blockIdx);
        auto end = graph->sEnd();
        while (it != end) {
            size_t successorIdx = (*it)->getIdx();
            if (!graph->isEndBlock(successorIdx)) {
                dst.add(successorIdx);
            }
            ++it;
        }
    }

    CfGraph<SkrInstruction*>* graph;
    std::vector<RCABlock> annotatedBlocks;
};
