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

    class OnGraphCreatedListener {
    public:
        virtual ~OnGraphCreatedListener() = default;
        virtual void onCreated(const char* funName, int iteration, CfgGraph<SkrInstruction*>* graph) = 0;
    };

    SkrOptimizer(Allocator& a1, SkrFunction* rawFunc, OnGraphCreatedListener* onGraphCreated = nullptr)
        : raw(rawFunc->getInstructions().toVector())
        , rawFunc(rawFunc)
        , a1(a1)
        , onGraphCreatedListener(onGraphCreated) {  }

    SkrFunction* optimize(Config config) {
        auto* initialGraph = CfgBuilder<SkrInstruction>().build_delGraphWhenDone(raw);
        notifyGraphCreated(rawFunc->getName(), 0, initialGraph);
        delete initialGraph;

        if (config.constantFolding) {
            ConstantFolding::run(a1, raw);
            filterNullptrs();
        }

        auto* graph = CfgBuilder<SkrInstruction>().build_delGraphWhenDone(raw);
        notifyGraphCreated(rawFunc->getName(), 1, graph);

        auto bodyBa = BoundArray<SkrInstruction*>::fromVector(raw, a1);

        delete graph;
        return a1.create<SkrFunction>(
            rawFunc->getName(),
            rawFunc->getParams(),
            bodyBa,
            rawFunc->getResultIdentifier()
        );
    }

private:
    void filterNullptrs() {
        size_t offset = 0;
        for (size_t i = 0; i < raw.size(); i++) {
            if (raw[i] == nullptr) {
                offset++;
            } else {
                raw[i - offset] = raw[i];
            }
        }
        
        if (offset > 0) {
            raw.resize(raw.size() - offset);
        }
    }

    void notifyGraphCreated(const char* funName, int iteration, CfgGraph<SkrInstruction*>* graph) {
        if (onGraphCreatedListener) {
            onGraphCreatedListener->onCreated(funName, iteration, graph);
        }
    }

    std::vector<SkrInstruction*> raw;
    SkrFunction* rawFunc;
    Allocator& a1;
    OnGraphCreatedListener* onGraphCreatedListener = nullptr;
};