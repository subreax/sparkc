#pragma once
#include "../../../common/cfg/CfGraph.h"
#include "../../instr/everything.h"
#include "VarSet.h"
#include "DSEBlock.h"
#include "../copyprop/Uniqueue.h"

class LivenessAnalysis {
public:
    LivenessAnalysis(const CfGraph<SkrInstruction*>* graph)
        : graph(graph)
        , blocks(graph->getBlocksCount()) {  }

    void run() {
        Uniqueue<size_t> workQueue;
        for (size_t i = blocks.size() - 1; i > 0; i--) {
            workQueue.add(i);
        }

        while (workQueue.isNotEmpty()) {
            size_t idx = workQueue.peek();
            workQueue.pop();

            DSEBlock oldBlock = blocks[idx];
            VarSet incoming;
            meet(idx, incoming);
            transfer(idx, incoming);
            if (oldBlock != blocks[idx]) {
                addAllPredecessors(idx, workQueue);
            }
        }
    }

    const DSEBlock& getAnnotation(CfgBlock<SkrInstruction*>* block) const {
        return blocks[block->getIdx()];
    }

private:
    void transfer(size_t blockIdx, const VarSet& incoming) {
        VarSet vars = incoming;

        auto& annotated = blocks[blockIdx];
        annotated.clear();

        const auto& body = getBlock(blockIdx)->getBody();
        for (int i = body.size() - 1; i >= 0; i--) {
            auto* instr = body[i];
            annotated.addInstrVars(vars);

            if (instr->kind == SkrInstruction::Kind::Binary) {
                auto* it = (SkrBinary*) instr;
                vars.kill(it->getDst());
                vars.generateIfVar(it->getLeft());
                vars.generateIfVar(it->getRight());
            }
            else if (instr->kind == SkrInstruction::Kind::Copy) {
                auto* it = (SkrCopy*) instr;
                vars.kill(it->getTo());
                vars.generateIfVar(it->getFrom());
            }
            else if (instr->kind == SkrInstruction::Kind::Branch) {
                auto* it = (SkrBranch*) instr;
                vars.generateIfVar(it->getLeft());
                vars.generateIfVar(it->getRight());
            }
            else if (instr->kind == SkrInstruction::Kind::FunCall) {
                auto* it = (SkrFunCall*) instr;
                vars.kill(it->getRetVar());
                for (auto* arg : it->getArgs()) {
                    vars.generateIfVar(arg);
                }
            }
            else if (instr->kind == SkrInstruction::Kind::Float2Int) {
                auto* it = (SkrFloat2Int*) instr;
                vars.kill(it->getDst());
                vars.generateIfVar(it->getSrc());
            }
            else if (instr->kind == SkrInstruction::Kind::Int2Float) {
                auto* it = (SkrInt2Float*) instr;
                vars.kill(it->getDst());
                vars.generateIfVar(it->getSrc());
            }
        }
        
        annotated.setBlockVars(vars);
        annotated.reverseInstructions();
    }

    void meet(size_t blockIdx, VarSet& outVars) {
        outVars.clear();

        auto it = graph->successorsIterator(blockIdx);
        auto end = graph->sEnd();
        while (it != end) {
            auto sIdx = (*it)->getIdx();
            outVars.addAll(blocks[sIdx].getBlockVars());
            ++it;
        }
    }

    const CfgBlock<SkrInstruction*>* getBlock(size_t idx) const {
        return graph->getBlock(idx);
    }

    
    void addAllPredecessors(size_t blockIdx, Uniqueue<size_t>& out) {
        auto it = graph->precedessorsIterator(blockIdx);
        auto end = graph->pEnd();
        while (it != end) {
            out.add((*it)->getIdx());
            ++it;
        }
    }
    
    std::vector<DSEBlock> blocks;
    const CfGraph<SkrInstruction*>* graph;
};
