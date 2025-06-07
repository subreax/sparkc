#pragma once
#include "../../common/alloc/LinearAllocator.h"
#include "../instr/everything.h"
#include "../SkrFunction.h"
#include "ConstantFolding.h"
#include "SkrCfgOverloadings.h"
#include "../../common/cfg/CfgBuilder.h"

class SkrOptimizer {
public:
    struct Config {
        bool constantFolding = true;
        bool deadCodeElimination = true;
        bool copyPropagation = true;
        bool deadStoreElimination = true;
    };

    SkrOptimizer(Allocator& a1, SkrFunction* rawFunc)
        : raw(rawFunc->getInstructions().toVector())
        , rawFunc(rawFunc)
        , a1(a1) {  }

    SkrFunction* optimize(Config config) {
        if (config.constantFolding) {
            ConstantFolding::run(a1, raw);
        }

        auto* graph = CfgBuilder<SkrInstruction>().build_delGraphWhenDone(raw);

        auto bodyBa = BoundArray<SkrInstruction*>::fromVector(raw, a1);
        return a1.create<SkrFunction>(
            rawFunc->getName(),
            rawFunc->getParams(),
            bodyBa,
            rawFunc->getResultIdentifier()
        );

        delete graph;
    }

private:
    std::vector<SkrInstruction*> raw;
    SkrFunction* rawFunc;
    Allocator& a1;
};