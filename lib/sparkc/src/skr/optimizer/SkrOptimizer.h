#pragma once
#include <functional>
#include "ConstantFolding.h"
#include "UnreachableCodeElimination.h"
#include "copyprop/CopyPropagation.h"
#include "dselim/DeadStoreElimination.h"
#include "sparkc/common/alloc/LinearAllocator.h"
#include "sparkc/common/cfg/CfgBuilder.h"
#include "sparkc/skr/SkrFactory.h"
#include "sparkc/skr/SkrFunction.h"
#include "sparkc/skr/instr/everything.h"
#include "sparkc/skr/optimizer/SkrOptimizerConfig.h"

class SkrOptimizer {
public:
    using OnCfgGraphCreatedListener = std::function<void(StringRef funName, int iteration, CfGraph<SkrInstruction*>* graph)>;
    static constexpr size_t MAX_ITERATIONS = 50;

    SkrOptimizer(
        Allocator& a1,
        SkrFunction* rawFunc,
        OnCfgGraphCreatedListener onGraphCreated = nullListener
    )
        : raw(rawFunc->getInstructions().toVector())
        , rawFunc(rawFunc)
        , a1(a1)
        , onCfgCreatedListener(onGraphCreated) { }

    SkrFunction* optimize(const SkrOptimizerConfig& config) {
        std::vector<SkrInstruction*> optimized = raw;
        SkrFactory skrf(a1);

        for (size_t i = 1; i <= MAX_ITERATIONS; i++) {
            if (config.constantFolding) {
                ConstantFolding::run(skrf, optimized);
            }

            auto* graph = CfgBuilder<SkrInstruction>().build_delGraphWhenDone(optimized);
            if (i == 1) {
                onCfgCreatedListener(rawFunc->getName(), 0, graph);
            }

            if (config.deadCodeElimination) {
                UnreachableCodeElimination(graph).run();
            }
            if (config.copyPropagation) {
                CopyPropagation(graph, a1).run();
            }
            if (config.deadStoreElimination) {
                DeadStoreElimination(graph, rawFunc->getRetVar()).run();
            }

            onCfgCreatedListener(rawFunc->getName(), i, graph);

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
            rawFunc->getRetVar()
        );
    }

private:
    static void nullListener(StringRef funName, int iteration, CfGraph<SkrInstruction*>* graph) { }

    std::vector<SkrInstruction*> raw;
    SkrFunction* rawFunc;
    Allocator& a1;
    OnCfgGraphCreatedListener onCfgCreatedListener = nullListener;
};
