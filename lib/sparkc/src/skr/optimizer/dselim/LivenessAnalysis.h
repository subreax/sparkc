#pragma once
#include "../Uniqueue.h"
#include "DSEBlock.h"
#include "VarSet.h"
#include "sparkc/skr/optimizer/SkrCfg.h"
#include "sparkc/skr/instr/everything.h"

class LivenessAnalysis {
public:
    LivenessAnalysis(const SkrCfg& graph, const SkrVar* retVar)
        : graph(graph)
        , annotations(graph.getSize())
        , retVar(retVar) { }

    void run() {
        Uniqueue<size_t> workQueue;

        addRetVarToEndBlock();

        // skip end block
        for (size_t i = annotations.size() - 2; i > 0; i--) {
            workQueue.add(i);
        }

        DSEBlock oldAnnotation;
        VarSet incoming;
        while (workQueue.isNotEmpty()) {
            size_t idx = workQueue.peek();
            workQueue.pop();

            oldAnnotation = annotations[idx];
            // incoming.generate(retVar);
            meet(idx, incoming);
            transfer(idx, incoming);
            if (oldAnnotation != annotations[idx]) {
                addAllPredecessors(idx, workQueue);
            }
            incoming.clear();
        }
    }

    const DSEBlock& getAnnotation(size_t blockIdx) const {
        return annotations[blockIdx];
    }

private:
    void addRetVarToEndBlock() {
        VarSet vars;
        vars.generate(retVar);
        annotations[graph.getSize() - 1].setBlockVars(vars);
    }

    void transfer(size_t blockIdx, const VarSet& incoming) {
        VarSet vars = incoming;

        auto& block = graph[blockIdx];
        auto& annotated = annotations[blockIdx];
        annotated.clear();
        annotated.resize(block.getBody().size());

        const auto& body = block.getBody();
        for (int i = body.size() - 1; i >= 0; i--) {
            auto* instr = body[i];
            annotated[i] = vars;

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
            else if (instr->kind == SkrInstruction::Kind::CopyToOffset) {
                auto* it = (SkrCopyToOffset*) instr;
                vars.generateIfVar(it->getTo());
                vars.generateIfVar(it->getFrom());
            }
            else if (instr->kind == SkrInstruction::Kind::CopyFromOffset) {
                auto* it = (SkrCopyFromOffset*) instr;
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
    }

    void meet(size_t blockIdx, VarSet& outVars) {
        auto it = graph.successors(blockIdx);
        while (it.hasNext()) {
            auto sIdx = it.nextIdx();
            outVars.addAll(annotations[sIdx].getBlockVars());
        }
    }

    void addAllPredecessors(size_t blockIdx, Uniqueue<size_t>& out) {
        auto it = graph.predecessors(blockIdx);
        while (it.hasNext()) {
            out.add(it.nextIdx());
        }
    }

    std::vector<DSEBlock> annotations;
    const SkrCfg& graph;
    const SkrVar* retVar;
};
