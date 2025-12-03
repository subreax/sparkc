#pragma once
#include "ConstantFolding.h"
#include "sparkc/skr/optimizer/SkrOptOnCfgCreatedListener.h"
#include "UnreachableCodeElimination.h"
#include "copyprop/CopyPropagation.h"
#include "dselim/DeadStoreElimination.h"
#include "sparkc/common/alloc/LinearAllocator.h"
#include "sparkc/common/cfg/CfgBuilder.h"
#include "sparkc/skr/SkrFunction.h"
#include "sparkc/skr/instr/everything.h"
#include "sparkc/skr/optimizer/SkrOptimizerConfig.h"

class SkrOptimizer {
public:
    static constexpr size_t MAX_ITERATIONS = 50;

    SkrOptimizer(
        Allocator& a1,
        SkrFunction* rawFunc,
        SkrOptOnCfgCreatedListener* onGraphCreated = nullptr)
        : raw(rawFunc->getInstructions().toVector()), rawFunc(rawFunc), a1(a1),
          onCfgCreatedListener(onGraphCreated) {}

    SkrFunction* optimize(const SkrOptimizerConfig& config) {
        if (onCfgCreatedListener != nullptr) {
            auto* initialGraph =
                CfgBuilder<SkrInstruction>().build_delGraphWhenDone(raw);
            notifyGraphCreated(rawFunc->getName(), 0, initialGraph);
            delete initialGraph;
        }

        std::vector<SkrInstruction*> optimized = raw;

        for (size_t i = 1; i <= MAX_ITERATIONS; i++) {
            if (config.constantFolding) {
                ConstantFolding::run(a1, optimized);
            }

            auto* graph =
                CfgBuilder<SkrInstruction>().build_delGraphWhenDone(optimized);

            if (config.deadCodeElimination) {
                UnreachableCodeElimination(graph).run();
            }
            if (config.copyPropagation) {
                CopyPropagation(graph, a1).run();
            }
            if (config.deadStoreElimination) {
                DeadStoreElimination(graph, rawFunc->getRetVar()).run();
            }

            notifyGraphCreated(rawFunc->getName(), i, graph);

            optimized.clear();
            graph->toPlain(optimized);
            delete graph;

            if (raw == optimized) {
                break;
            }

            raw = optimized;
        }

        auto bodyBa = BoundArray<SkrInstruction*>::fromVector(raw, a1);
        return a1.create<SkrFunction>(
            rawFunc->getName(),
            rawFunc->getParams(),
            bodyBa,
            rawFunc->getRetVar());
    }

private:
    void notifyGraphCreated(
        StringRef funName, int iteration, CfGraph<SkrInstruction*>* graph) {
        if (onCfgCreatedListener != nullptr) {
            onCfgCreatedListener->onCfgCreated(funName, iteration, graph);
        }
    }

    std::vector<SkrInstruction*> raw;
    SkrFunction* rawFunc;
    Allocator& a1;
    SkrOptOnCfgCreatedListener* onCfgCreatedListener = nullptr;
};