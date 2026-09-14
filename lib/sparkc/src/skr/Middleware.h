#pragma once
#include <vector>
#include "sparkc/frontend/ast/everything.h"
#include "sparkc/skr/SkrStaticVar.h"
#include "sparkc/symbol/SymbolTable.h"
#include "sparkc/type/TypeTable.h"
#include "sparkc/common/IdentifierGen.h"
#include "sparkc/common/LabelGen.h"
#include "sparkc/SparkStageCallback.h"
#include "SkrEmitter.h"
#include "optimizer/SkrOptimizer.h"

class Middleware {
public:
    Middleware(
        SymbolTable& symTable,
        TypeTable& typeTable,
        IdentifierGen& idGen,
        LabelGen& labelGen,
        SkrFactory& skrf,
        SkrFactory& skrfShared,
        SparkStageCallback& stageCallback,
        SkrOptimizerConfig skrOptimizerConfig
    )
        : symTable(symTable)
        , typeTable(typeTable)
        , idGen(idGen)
        , labelGen(labelGen)
        , skrf(skrf)
        , skrfShared(skrfShared)
        , stageCallback(stageCallback)
        , skrOptimizerConfig(skrOptimizerConfig) {
        skrsBuf.reserve(16);
    }

    SkrProgItem* process(AstProgItem* astProgItem) {
        auto* item = optimize(build(astProgItem));
        if (item == nullptr) {
            return nullptr;
        }

        if (item->kind == SkrProgItem::Kind::Var) {
            auto* staticVar = (SkrStaticVar*) item;
            const auto& init = staticVar->getInitializer();
            for (auto* initInstr : init) {
                initFnBody.emplace_back(initInstr);
            }
        }

        stageCallback.onEmitSkr(item);
        return item;
    }

    SkrFunction* declareAndBuildInitFunction(const char* funName_) {
        if (initFnBody.empty()) {
            return nullptr;
        }

        // todo: make function void
        StringRef funName = idGen.copy(funName_);
        StringRef retVarId = idGen.unique(funName);
        symTable.declareVar(retVarId, symTable.getTypeFactory().int_(), false);
        symTable.declareFunc(funName, symTable.getTypeFactory().int_(), { });
        // todo: mixed pools (factories). Instructions created through skrfShared,
        // function created through skrf, and all the optimizations work with skrf.
        // Probably better to copy instructions from skrfShared to skrf
        auto* initFun = skrf.function(
            idGen.copy(funName),
            { },
            initFnBody,
            skrf.var(retVarId)
        );
        initFnBody.clear();
        auto* optimizedInitFun = (SkrFunction*) optimize(initFun);
        stageCallback.onEmitSkr(optimizedInitFun);
        return optimizedInitFun;
    }

private:
    SkrProgItem* build(AstProgItem* astProgItem) {
        skrsBuf.clear();

        if (astProgItem->kind == AstProgItem::Kind::Struct) {
            return nullptr;
        }

        auto* skrProgItem = SkrEmitter::emit(
            astProgItem,
            resolveSkrFactory(astProgItem->kind),
            symTable,
            typeTable,
            idGen,
            labelGen,
            skrsBuf
        );

        if (skrProgItem == nullptr) {
            sparkError("SparkCompiler", "Unknown AstProgItem kind: %d", astProgItem->kind);
            return nullptr;
        }

        return skrProgItem;
    }

    SkrProgItem* optimize(SkrProgItem* item) {
        if (item != nullptr && item->kind == SkrProgItem::Kind::Function) {
            return SkrOptimizer(
                       symTable,
                       skrf,
                       (SkrFunction*) item,
                       [&](StringRef funName, int iteration, SkrCfg& graph) {
                           stageCallback.onCfgCreated(funName, iteration, graph);
                       }
            ).optimize(skrOptimizerConfig);
        }
        return item;
    }

    SkrFactory& resolveSkrFactory(AstProgItem::Kind itemKind) {
        if (itemKind == AstProgItem::Kind::Variable) {
            return skrfShared;
        }
        return skrf;
    }

    SymbolTable& symTable;
    TypeTable& typeTable;
    IdentifierGen& idGen;
    LabelGen& labelGen;
    SkrFactory& skrf;
    SkrFactory& skrfShared; // to save static vars from erasing after this stage
    SparkStageCallback& stageCallback;
    SkrOptimizerConfig skrOptimizerConfig;
    std::vector<SkrInstruction*> initFnBody;

    std::vector<SkrInstruction*> skrsBuf;
};