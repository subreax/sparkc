#pragma once
#include "../../common/alloc/LinearAllocator.h"
#include "../instr/everything.h"
#include "../SkrFunction.h"
#include "../../common/cfg/CfgBuilder.h"
#include "SkrCfgOverloadings.h"
#include "ConstantFolding.h"
#include "UnreachableCodeElimination.h"
#include "copyprop/CopyPropagation.h"

class SkrOptimizer {
public:
    struct Config {
        bool constantFolding = true;
        bool deadCodeElimination = true;
        bool copyPropagation = true;
        bool deadStoreElimination = true;
    };

    class OnGraphCreatedListener {
    public:
        virtual ~OnGraphCreatedListener() = default;
        virtual void onCreated(const char* funName, int iteration, CfGraph<SkrInstruction*>* graph) = 0;
    };

    SkrOptimizer(Allocator& a1, SkrFunction* rawFunc, OnGraphCreatedListener* onGraphCreated = nullptr)
        : raw(rawFunc->getInstructions().toVector())
        , rawFunc(rawFunc)
        , a1(a1)
        , onGraphCreatedListener(onGraphCreated) {  }

    SkrFunction* optimize(Config config) {
        if (onGraphCreatedListener != nullptr) {
            auto* initialGraph = CfgBuilder<SkrInstruction>().build_delGraphWhenDone(raw);
            notifyGraphCreated(rawFunc->getName(), 0, initialGraph);
            delete initialGraph;
        }

        for (size_t i = 1; i <= 4; i++) {
            if (config.constantFolding) {
                ConstantFolding::run(a1, raw);
            }
    
            auto* graph = CfgBuilder<SkrInstruction>().build_delGraphWhenDone(raw);
            
            if (config.deadCodeElimination) {
                UnreachableCodeElimination(graph).run();
            }
            if (config.copyPropagation) {
                CopyPropagation(graph, a1).run();
            }

            notifyGraphCreated(rawFunc->getName(), i, graph);
            raw.clear();
            graph->toPlain(raw);

            delete graph;
        }


        auto bodyBa = BoundArray<SkrInstruction*>::fromVector(raw, a1);
        return a1.create<SkrFunction>(
            rawFunc->getName(),
            rawFunc->getParams(),
            bodyBa,
            rawFunc->getResultIdentifier()
        );
    }

private:
    void notifyGraphCreated(const char* funName, int iteration, CfGraph<SkrInstruction*>* graph) {
        if (onGraphCreatedListener != nullptr) {
            onGraphCreatedListener->onCreated(funName, iteration, graph);
        }
    }

    std::vector<SkrInstruction*> raw;
    SkrFunction* rawFunc;
    Allocator& a1;
    OnGraphCreatedListener* onGraphCreatedListener = nullptr;
};