#include "CopyPropagation.h"
#include "../SkrOptimizerUtils.h"
#include "ReachingCopiesAnalysis.h"

CopyPropagation::CopyPropagation(
    SkrFactory& skrf,
    SkrCfg& graph
)
    : skrf(skrf)
    , graph(graph) { }

void CopyPropagation::run() {
    ReachingCopiesAnalysis rca(graph);
    rca.run();

    for (size_t i = 1; i < graph.getSize() - 1; i++) {
        auto& block = graph[i];
        auto& annotated = rca.getAnnotated(i);

        auto& body = block.getBody();
        const auto instrCount = body.size();
        for (size_t instrIdx = 0; instrIdx < instrCount; instrIdx++) {
            body[instrIdx] = rewriteInstruction(block, annotated, instrIdx);
        }
        SkrOptimizerUtils::filterNullptrs(body);
    }
}

SkrInstruction* CopyPropagation::rewriteInstruction(
    SkrCfgBlock& block,
    RCABlock& annotated,
    size_t instrIdx
) {
    auto* instr = block[instrIdx];
    const auto& copies = annotated.instructionAnnotations[instrIdx];
    if (instr->kind == SkrInstruction::Kind::Binary) {
        auto* it = (SkrBinary*) instr;
        auto* left = replace(it->getLeft(), copies);
        auto* right = replace(it->getRight(), copies);
        if (left != it->getLeft() || right != it->getRight()) {
            return skrf.binary(it->getDst(), left, it->getOperator(), right);
        }
    }
    else if (instr->kind == SkrInstruction::Kind::Copy) {
        auto* it = (SkrCopy*) instr;
        auto* from = replace(it->getFrom(), copies);
        if (*from == *it->getTo()) {
            return nullptr;
        }
        if (from != it->getFrom()) {
            return skrf.copy(it->getTo(), from);
        }
    }
    else if (instr->kind == SkrInstruction::Kind::CopyToOffset) {
        // CopyToOffset  4(color) = x
        // CopyToOffset  4(color) = replaced

        auto* it = (SkrCopyToOffset*) instr;
        auto* from = replace(it->getFrom(), copies);
        if (*from != *it->getFrom()) {
            return skrf.copyToOffset(it->getTo(), it->getToOffset(), from);
        }
    }
    else if (instr->kind == SkrInstruction::Kind::CopyFromOffset) {
        // CopyFromOffset   x = 4(color)
        // CopyFromOffset   x = 4(replaced)

        auto* it = (SkrCopyFromOffset*) instr;
        if (!it->getFrom()->isVar()) {
            sparkError("CopyPropagation", "Wrong 'from' type, expected var");
        }

        auto* from = (SkrVar*) replace(it->getFrom(), copies);
        if (*from != *it->getFrom()) {
            return skrf.copyFromOffset(it->getTo(), from, it->getFromOffset());
        }
    }
    else if (instr->kind == SkrInstruction::Kind::FunCall) {
        auto* it = (SkrFunCall*) instr;

        std::vector<SkrValue*> newArgs = replaceArgs(it->getArgs(), copies);
        if (it->getArgs() != newArgs) {
            return skrf.funCall(it->getName(), newArgs, it->getRetVar());
        }
    }
    else if (instr->kind == SkrInstruction::Kind::Float2Int) {
        auto* it = (SkrFloat2Int*) instr;
        auto* src = replace(it->getSrc(), copies);
        if (*src != *it->getSrc()) {
            return skrf.float2Int(it->getDst(), src);
        }
    }
    else if (instr->kind == SkrInstruction::Kind::Int2Float) {
        auto* it = (SkrInt2Float*) instr;
        auto* src = replace(it->getSrc(), copies);
        if (*src != *it->getSrc()) {
            return skrf.int2Float(it->getDst(), src);
        }
    }

    return instr;
}

std::vector<SkrValue*> CopyPropagation::replaceArgs(
    const BoundArray<SkrValue*>& args,
    const ReachingCopies& copies
) {
    std::vector<SkrValue*> newArgs;
    newArgs.reserve(args.size());
    for (size_t i = 0; i < args.size(); i++) {
        newArgs.emplace_back(replace(args[i], copies));
    }
    return newArgs;
}

SkrValue* CopyPropagation::replace(
    SkrValue* value,
    const ReachingCopies& copies
) {
    if (value->isConst()) {
        return value;
    }

    auto* copy = copies.findByDst(value);
    if (copy != nullptr) {
        return copy->getFrom();
    }
    else {
        return value;
    }
}
