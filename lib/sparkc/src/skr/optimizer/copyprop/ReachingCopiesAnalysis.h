#pragma once
#include "../Uniqueue.h"
#include "RCABlock.h"
#include "ReachingCopies.h"
#include "sparkc/skr/optimizer/SkrCfg.h"
#include "sparkc/skr/instr/everything.h"
#include "sparkc/common/cfg/CfgUtils.h"

class ReachingCopiesAnalysis {
public:
    ReachingCopiesAnalysis(SkrCfg& graph)
        : graph(graph)
        , annotatedBlocks(graph.getSize()) { }

    void run() {
        const ReachingCopies identity = getAllCopies();

        Uniqueue<size_t> pendingBlocks;
        for (size_t i = 1; i < annotatedBlocks.size() - 1; i++) {
            auto& ab = getAnnotated(i);
            ab.blockAnnotation = identity;
            pendingBlocks.add(i);
        }

        ReachingCopies incomingCopies;
        incomingCopies.reserve(identity.size());
        RCABlock oldAnnotation;

        size_t i;
        for (i = 0; i < MAX_ITERATIONS && pendingBlocks.isNotEmpty(); i++) {
            size_t blockIdx = getNext(pendingBlocks);
            RCABlock& annotation = getAnnotated(blockIdx);

            oldAnnotation = annotation;
            meet(blockIdx, identity, incomingCopies);
            transfer(blockIdx, incomingCopies);
            if (annotation != oldAnnotation) {
                addAllSuccessors(blockIdx, pendingBlocks);
            }

            incomingCopies.clear();
        }

        if (i == MAX_ITERATIONS) {
            sparkError("ReachingCopiesAnalysis", "Analysis was stuck in loop");
        }
    }

    RCABlock& getAnnotated(size_t idx) {
        return annotatedBlocks[idx];
    }

    const RCABlock* getAnnotated(size_t idx) const {
        return &annotatedBlocks[idx];
    }

    const std::vector<RCABlock>& getResult() const {
        return annotatedBlocks;
    }

private:
    void meet(
        size_t blockIdx,
        const ReachingCopies& identity,
        ReachingCopies& out
    ) {
        out = identity;
        auto it = graph.predecessors(blockIdx);
        while (it.hasNext()) {
            auto [predIdx, predecessor] = it.next();
            if (!CfgUtils::isBeginBlock(graph, predIdx)) {
                out.intersect(getAnnotated(predIdx).blockAnnotation);
            }
            else {
                out.clear();
                return;
            }
        }
    }

    void transfer(size_t blockIdx, const ReachingCopies& incomingCopies) {
        ReachingCopies currentCopies(incomingCopies);
        auto& block = graph[blockIdx];
        auto& annotated = getAnnotated(blockIdx);
        annotated.clear();

        for (auto* instr : block.getBody()) {
            annotated.addInstructionAnnotation(currentCopies);

            if (instr->kind == SkrInstruction::Kind::Copy) {
                auto* copyInstr = (SkrCopy*) instr;
                if (currentCopies.contains(copyInstr->getFrom(), copyInstr->getTo())) {
                    continue;
                }

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
        annotated.setBlockAnnotation(currentCopies);
    }

    ReachingCopies getAllCopies() {
        const auto& blocks = graph.getNodes();
        ReachingCopies result;
        for (auto& block : blocks) {
            getAllCopies(block, result);
        }
        return result;
    }

    void getAllCopies(const SkrCfgBlock& block, ReachingCopies& out) {
        for (auto* skr : block.getBody()) {
            if (skr->kind == SkrInstruction::Kind::Copy) {
                auto* skrCopy = (SkrCopy*) skr;
                out.add(skrCopy);
            }
        }
    }

    void addAllSuccessors(size_t blockIdx, Uniqueue<size_t>& dst) {
        auto it = graph.successors(blockIdx);
        while (it.hasNext()) {
            auto [successorIdx, block] = it.next();
            if (!CfgUtils::isEndBlock(graph, successorIdx)) {
                dst.add(successorIdx);
            }
        }
    }

    size_t getNext(Uniqueue<size_t>& uniqueue) const {
        auto elem = uniqueue.peek();
        uniqueue.pop();
        return elem;
    }

    static constexpr size_t MAX_ITERATIONS = 300;

    SkrCfg& graph;
    std::vector<RCABlock> annotatedBlocks;
};
