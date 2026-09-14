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
#include "sparkc/skr/optimizer/SkrCfg.h"
#include "sparkc/skr/optimizer/SkrOptimizerConfig.h"
#include "sparkc/common/cfg/CfgUtils.h"
#include "sparkc/symbol/SymbolTable.h"

class SkrOptimizer {
public:
    using OnCfgGraphCreatedListener = std::function<void(StringRef funName, int iteration, SkrCfg& graph)>;
    static constexpr size_t MAX_ITERATIONS = 50;

    SkrOptimizer(
        SymbolTable& symTable,
        SkrFactory& skrf,
        SkrFunction* rawFunc,
        OnCfgGraphCreatedListener onGraphCreated = nullListener
    )
        : symTable(symTable)
        , initial(rawFunc->getInstructions().toVector())
        , rawFunc(rawFunc)
        , skrf(skrf)
        , onCfgCreatedListener(onGraphCreated) { }

    SkrFunction* optimize(const SkrOptimizerConfig& config) {
        std::vector<SkrInstruction*> optimized = initial;

        {
            SkrCfg graph = CfgBuilder<SkrInstruction*>::build(initial);
            onCfgCreatedListener(rawFunc->getName(), -1, graph);
        }

        for (size_t i = 1; i <= MAX_ITERATIONS; i++) {
            if (config.constantFolding) {
                ConstantFolding::run(skrf, optimized);
            }

            SkrCfg graph = CfgBuilder<SkrInstruction*>::build(optimized);
            if (i == 1) {
                onCfgCreatedListener(rawFunc->getName(), 0, graph);
            }

            if (config.deadCodeElimination) {
                UnreachableCodeElimination(graph).run();
            }
            if (config.copyPropagation) {
                CopyPropagation(symTable, skrf, graph).run();
            }
            if (config.deadStoreElimination) {
                DeadStoreElimination(symTable, graph, rawFunc->getRetVar()).run();
            }

            onCfgCreatedListener(rawFunc->getName(), i, graph);

            optimized.clear();
            CfgUtils::graphToPlain(graph, optimized);

            if (initial == optimized) {
                break;
            }

            initial = optimized;
        }

        return skrf.function(
            rawFunc->getName(),
            rawFunc->getParams(),
            skrf.copyInstructions(optimized),
            rawFunc->getRetVar()
        );
    }

private:
    static void nullListener(StringRef, int, SkrCfg&) { }

    SymbolTable& symTable;
    std::vector<SkrInstruction*> initial;
    SkrFunction* rawFunc;
    SkrFactory& skrf;
    OnCfgGraphCreatedListener onCfgCreatedListener = nullListener;
};
