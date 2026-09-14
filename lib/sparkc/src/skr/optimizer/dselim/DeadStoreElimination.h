#pragma once
#include "../SkrOptimizerUtils.h"
#include "LivenessAnalysis.h"

class DeadStoreElimination {
public:
    DeadStoreElimination(SymbolTable& symTable, SkrCfg& graph, const SkrVar* retVar)
        : symTable(symTable)
        , graph(graph)
        , retVar(retVar) { }

    void run() {
        LivenessAnalysis la(symTable, graph, retVar);
        la.run();

        for (size_t blockIdx = 0; blockIdx < graph.getSize(); blockIdx++) {
            auto& block = graph[blockIdx];
            const DSEBlock& annotation = la.getAnnotation(blockIdx);
            processBlock(block, annotation);
        }
    }

private:
    void processBlock(SkrCfgBlock& block, const DSEBlock& annotation) {
        auto& body = block.getBody();
        for (size_t i = 0; i < body.size(); i++) {
            if (isDeadStore(body[i], annotation[i])) {
                body[i] = nullptr;
            }
        }
        SkrOptimizerUtils::filterNullptrs(body);
    }

    bool isDeadStore(SkrInstruction* instr, const VarSet& annotation) {
        if (instr->kind == SkrInstruction::Kind::Copy) {
            auto* it = (SkrCopy*) instr;
            return !annotation.contains(it->getTo());
        }
        else if (instr->kind == SkrInstruction::Kind::CopyToOffset) {
            auto* it = (SkrCopyToOffset*) instr;
            return !annotation.contains(it->getTo());
        }
        else if (instr->kind == SkrInstruction::Kind::CopyFromOffset) {
            auto* it = (SkrCopyFromOffset*) instr;
            return !annotation.contains(it->getTo());
        }
        else if (instr->kind == SkrInstruction::Kind::Binary) {
            auto* it = (SkrBinary*) instr;
            return !annotation.contains(it->getDst());
        }
        else if (instr->kind == SkrInstruction::Kind::Float2Int) {
            auto* it = (SkrFloat2Int*) instr;
            return !annotation.contains(it->getDst());
        }
        else if (instr->kind == SkrInstruction::Kind::Int2Float) {
            auto* it = (SkrInt2Float*) instr;
            return !annotation.contains(it->getDst());
        }
        // never delete function calls because:
        // 1. they can update static vars
        // 2. a funtion can be an external function that changes system behaviour
        /* else if (instr->kind == SkrInstruction::Kind::FunCall) {
            auto* it = (SkrFunCall*) instr;
            return !annotation.contains(it->getRetVar());
        } */

        return false;
    }

    SymbolTable& symTable;
    SkrCfg& graph;
    const SkrVar* retVar;
};
