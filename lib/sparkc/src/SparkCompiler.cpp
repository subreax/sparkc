#include "sparkc/SparkCompiler.h"
#include "SparkPools.h"

#include "sparkc/common/IdentifierGen.h"
#include "sparkc/common/LabelGen.h"
#include "sparkc/skr/SkrFactory.h"
#include "backend/rv/RvaFixer.h"
#include "backend/rv/RvaPseudoReplacer.h"
#include "backend/rv/Skr2RvaPseudo.h"
#include "sparkc/backend/rv/asm/RvAssembler.h"
#include "sparkc/backend/rv/instr/RvaInstruction.h"

#include "SparkBuildContext.h"

#include "frontend/Frontend.h"
#include "skr/Middleware.h"
#include "backend/rv/RvBackend.h"

#include "SparkInitContextImpl.h"

static SparkPools* pools = nullptr;

static SparkStageCallback nullStageCallback;
static SparkStageCallback* stageCallback = &nullStageCallback;

static SparkRuntime runtime;
static SkrOptimizerConfig skrOptimizerConfig;
static SparkBuildStage finalBuildStage;

static const char* initFunName;

static std::vector<SparkCompiler::OnInitCallback> initCallbacks;

static BuildResult buildResult(SymbolTable& symTable, RvAssembler& assembler, uint8_t* outBin);

void SparkCompiler::init(const SparkCompilerConfig& config) {
    if (pools != nullptr) {
        sparkError("SparkCompiler", "SparkCompiler already initialized");
    }

    pools = new SparkPools(config.poolSize);
    runtime = config.runtime;
    if (config.stageCallback != nullptr) {
        stageCallback = config.stageCallback;
    }
    skrOptimizerConfig.constantFolding = config.optimizations & SPARK_OPT_CONSTANT_FOLDING;
    skrOptimizerConfig.copyPropagation = config.optimizations & SPARK_OPT_COPY_PROPAGATION;
    skrOptimizerConfig.deadCodeElimination = config.optimizations & SPARK_OPT_DEAD_CODE_ELIM;
    skrOptimizerConfig.deadStoreElimination = config.optimizations & SPARK_OPT_DEAD_STORE_ELIM;
    finalBuildStage = config.finalBuildStage;
    initFunName = config.initFunName;
}

void SparkCompiler::destroy() {
    delete pools;
    initCallbacks.clear();
    stageCallback = &nullStageCallback;
}

void SparkCompiler::addOnInitCallback(OnInitCallback cbk) {
    initCallbacks.emplace_back(std::move(cbk));
}

BuildResult SparkCompiler::build(const char* src, uint8_t* outBin, size_t outCap) {
    pools->reset();

    SparkBuildContext ctx(pools->shared, MemBlockRef(outCap, outBin));

    stageCallback->setSymbolTable(ctx.symTable);
    ctx.assembler.addExternalLabel(StringRef::cstr(SparkRuntime::divq15FunName), (void*) runtime.divq15);
    {
        SparkInitContextImpl initCtx(ctx);
        for (auto& cbk : initCallbacks) {
            cbk(initCtx);
        }
    }

    AstFactory astFactory(pools->pool1);
    SkrFactory skrFactory(pools->pool2);
    SkrFactory skrFactoryShared(pools->shared);

    Frontend frontend(src, astFactory, ctx.symTable, ctx.typeTable, ctx.idGen, *stageCallback);
    Middleware middleware(ctx.symTable, ctx.typeTable, ctx.idGen, ctx.labelGen, skrFactory, skrFactoryShared, *stageCallback, skrOptimizerConfig);
    RvBackend backend(pools->pool1, pools->pool2, ctx.symTable, ctx.symSize, ctx.idGen, *stageCallback);

    while (frontend.hasNext()) {
        pools->pool1.reset();
        pools->pool2.reset();

        // === AST ===
        AstProgItem* astProgItem = frontend.processNextItem();

        // === SKR ===
        if (finalBuildStage < SparkBuildStage::SKR) {
            continue;
        }

        SkrProgItem* skrProgItem = middleware.process(astProgItem);
        if (skrProgItem == nullptr) {
            continue;
        }

        // === RVA ===
        if (finalBuildStage < SparkBuildStage::RVA_Initial) {
            continue;
        }
        backend.emit(skrProgItem);

        if (finalBuildStage < SparkBuildStage::RVA_Replaced) {
            continue;
        }
        backend.replacePseudo();

        if (finalBuildStage < SparkBuildStage::RVA_Fixed) {
            continue;
        }
        const auto& rvas = backend.fix();

        if (finalBuildStage == SparkBuildStage::Bin) {
            ctx.assembler.compile(rvas);
        }
    }

    // todo: please refactor me
    if (finalBuildStage >= SparkBuildStage::SKR) {
        SkrFunction* initFn = middleware.declareAndBuildInitFunction(initFunName);
        if (initFn != nullptr && finalBuildStage >= SparkBuildStage::RVA_Initial) {
            backend.emit(initFn);
            if (finalBuildStage >= SparkBuildStage::RVA_Replaced) {
                backend.replacePseudo();
                if (finalBuildStage >= SparkBuildStage::RVA_Fixed) {
                    const auto& initRvas = backend.fix();
                    if (finalBuildStage == SparkBuildStage::Bin) {
                        ctx.assembler.compile(initRvas);
                    }
                }
            }
        }
    }

    ctx.assembler.link();
    auto res = buildResult(ctx.symTable, ctx.assembler, outBin);
    if (finalBuildStage == SparkBuildStage::Bin) {
        stageCallback->onBinary(res);
    }
    return res;
}

PoolsMemoryStats SparkCompiler::getMemoryUsage() {
    return pools->getMemoryUsage();
}

static BuildResult buildResult(SymbolTable& symTable, RvAssembler& assembler, uint8_t* outBin) {
    std::unordered_map<StringRef, BuildResult::Function> functions;

    auto publicLabels = assembler.getPublicLabels();
    for (const auto& label : publicLabels) {
        auto* type = symTable.get(label.value).getType();
        if (type->kind == SymbolType::Kind::Function) {
            auto fun = BuildResult::Function(
                label.offset,
                label.value,
                (SymbolFunctionType*) type
            );
            functions.emplace(label.value, fun);
        }
    }

    return BuildResult(outBin, assembler.getSize(), functions);
}
