#pragma once
#include "../SkrOptimizerUtils.h"
#include "LivenessAnalysis.h"

class DeadStoreElimination {
public:
    DeadStoreElimination(CfGraph<SkrInstruction*>* graph, const SkrVar* retVar)
        : graph(graph)
        , retVar(retVar) { }

    void run() {
        LivenessAnalysis la(graph);
        la.run();

        const auto& blocks = graph->getBlocks();
        for (CfgBlock<SkrInstruction*>* block : blocks) {
            const DSEBlock& annotation = la.getAnnotation(block);
            processBlock(block, annotation);
        }
    }

private:
    void processBlock(CfgBlock<SkrInstruction*>* block, const DSEBlock& annotation) {
        auto& body = block->getBody();
        for (size_t i = 0; i < body.size(); i++) {
            if (isDeadStore(body[i], annotation.getInstrVars()[i])) {
                body[i] = nullptr;
            }
        }
        SkrOptimizerUtils::filterNullptrs(body);
    }

    bool isDeadStore(SkrInstruction* instr, const VarSet& annotation) {
        if (instr->kind == SkrInstruction::Kind::Copy) {
            auto* it = (SkrCopy*) instr;
            return isNotRetVar(it->getTo()) && !annotation.contains(it->getTo());
        }
        else if (instr->kind == SkrInstruction::Kind::CopyToOffset) {
            auto* it = (SkrCopyToOffset*) instr;
            return isNotRetVar(it->getTo()) && !annotation.contains(it->getTo());
        }
        else if (instr->kind == SkrInstruction::Kind::CopyFromOffset) {
            auto* it = (SkrCopyFromOffset*) instr;
            return isNotRetVar(it->getTo()) && !annotation.contains(it->getTo());
        }
        else if (instr->kind == SkrInstruction::Kind::Binary) {
            auto* it = (SkrBinary*) instr;
            return isNotRetVar(it->getDst()) && !annotation.contains(it->getDst());
        }
        else if (instr->kind == SkrInstruction::Kind::Float2Int) {
            auto* it = (SkrFloat2Int*) instr;
            return isNotRetVar(it->getDst()) && !annotation.contains(it->getDst());
        }
        else if (instr->kind == SkrInstruction::Kind::Int2Float) {
            auto* it = (SkrInt2Float*) instr;
            return isNotRetVar(it->getDst()) && !annotation.contains(it->getDst());
        }
        else if (instr->kind == SkrInstruction::Kind::FunCall) {
            auto* it = (SkrFunCall*) instr;
            return isNotRetVar(it->getRetVar()) && !annotation.contains(it->getRetVar());
        }

        return false;
    }

    bool isNotRetVar(const SkrValue* value) {
        if (value->isVar()) {
            return *((const SkrVar*) value) != *retVar;
        }
        return true;
    }

    CfGraph<SkrInstruction*>* graph;
    const SkrVar* retVar;
};
