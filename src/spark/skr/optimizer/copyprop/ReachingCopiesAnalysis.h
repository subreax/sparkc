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

        while (pendingBlocks.isNotEmpty()) {
            size_t blockIdx = pendingBlocks.peek();
            pendingBlocks.pop();
            auto* ab = getAnnotated(blockIdx);

            auto oldAb = *ab;
            std::vector<SkrCopy*> incomingCopies;
            meet(blockIdx, allCopies, incomingCopies);
            transfer(blockIdx, incomingCopies);
            if (*ab != oldAb) {
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
                currentCopies.kill(copyInstr->getTo()->toSkrVar());
                currentCopies.add(copyInstr);
            }
            else if (instr->kind == SkrInstruction::Kind::Binary) {
                auto* binInstr = (SkrBinary*) instr;
                currentCopies.kill(binInstr->getDst()->toSkrVar());
            }
            else if (instr->kind == SkrInstruction::Kind::FunCall) {
                auto* callInstr = (SkrFunCall*) instr;
                currentCopies.kill(callInstr->getRetVar());
            }
        }
        annotated->blockCopies = currentCopies.getCopies();
    }

    void getAllCopies(std::vector<SkrCopy*>& out) {
        ReachingCopies copies;
        const auto& blocks = graph->getBlocks();
        for (auto* block : blocks) {
            for (auto* skr : block->getBody()) {
                if (skr->kind == SkrInstruction::Kind::Copy) {
                    copies.add((SkrCopy*) skr);
                }
            }
        }
        out = copies.getCopies();
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
