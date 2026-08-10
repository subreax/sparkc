#include "sparkc/SparkCompiler.h"
#include "SparkPools.h"

#include "sparkc/frontend/Frontend.h"
#include "skr/SkrEmitter.h"
#include "sparkc/skr/SkrFactory.h"
#include "skr/optimizer/SkrOptimizer.h"
#include "backend/rv/RvaFixer.h"
#include "backend/rv/RvaPseudoReplacer.h"
#include "backend/rv/Skr2RvaPseudo.h"
#include "sparkc/backend/rv/asm/RvAssembler.h"
#include "sparkc/backend/rv/instr/RvaInstruction.h"
#include "SparkInitContextImpl.h"

static SparkPools* pools = nullptr;
static uint8_t* outBin = nullptr;
static size_t outCap = 0;

static SparkStageCallback nullStageCallback;
static SparkStageCallback* stageCallback = &nullStageCallback;

static SparkRuntime runtime;
static SkrOptimizerConfig skrOptimizerConfig;
static SparkBuildStage finalBuildStage;

static std::vector<SparkCompiler::OnInitCallback> initCallbacks;

static BuildResult buildResult(SymbolTable& symTable, RvAssembler& assembler);

void SparkCompiler::init(const SparkCompilerConfig& config) {
    if (pools != nullptr) {
        sparkError("SparkCompiler", "SparkCompiler already initialized");
    }

    pools = new SparkPools(config.poolSize);
    outBin = config.outBin;
    outCap = config.outCap;
    runtime = config.runtime;
    stageCallback = config.stageCallback;
    skrOptimizerConfig.constantFolding = config.optimizations & SPARK_OPT_CONSTANT_FOLDING;
    skrOptimizerConfig.copyPropagation = config.optimizations & SPARK_OPT_COPY_PROPAGATION;
    skrOptimizerConfig.deadCodeElimination = config.optimizations & SPARK_OPT_DEAD_CODE_ELIM;
    skrOptimizerConfig.deadStoreElimination = config.optimizations & SPARK_OPT_DEAD_STORE_ELIM;
    finalBuildStage = config.finalBuildStage;
}

void SparkCompiler::destroy() {
}

void SparkCompiler::addOnInitCallback(OnInitCallback cbk) {
    initCallbacks.emplace_back(std::move(cbk));
}

BuildResult SparkCompiler::build(const char* src) {
    pools->reset();

    SymbolTable symTable(pools->shared);
    TypeTable typeTable(pools->shared);
    SymbolSize symSize(symTable, typeTable);
    IdentifierGen idGen(pools->shared);
    LabelGen labelGen(pools->shared);
    RvAssembler assembler(outBin, outCap);

    stageCallback->setSymbolTable(symTable);
    assembler.addExternalLabel(StringRef::cstr(SparkRuntime::divq15FunName), (void*) runtime.divq15);
    {
        SparkInitContextImpl initCtx(idGen, symTable, typeTable, assembler);
        for (auto& cbk : initCallbacks) {
            cbk(initCtx);
        }
    }

    AstFactory astFactory(pools->pool1);
    SkrFactory skrFactory(pools->pool2);
    Frontend frontend(src, astFactory, symTable, typeTable, idGen);

    std::vector<SkrInstruction*> skrsBuf;
    std::vector<RvaInstruction*> tempRvas;
    std::vector<RvaInstruction*> rvas;

    while (frontend.hasNext()) {
        pools->pool1.reset();
        pools->pool2.reset();
        skrsBuf.clear();
        tempRvas.clear();
        rvas.clear();

        AstProgItem* astProgItem = frontend.processNextItem();
        stageCallback->onAstBuild(astProgItem);

        if (astProgItem->kind != AstProgItem::Kind::Function) {
            continue;
        }

        SkrFunction* skrFunc = nullptr;
        if (finalBuildStage >= SparkBuildStage::SKR) {
            auto* astFunc = (AstFunction*) astProgItem;

            skrFunc = SkrEmitter::emit(
                astFunc,
                skrFactory,
                symTable,
                typeTable,
                idGen,
                labelGen,
                skrsBuf
            );

            skrFunc = SkrOptimizer(
                          pools->pool2,
                          skrFunc,
                          [&](StringRef funName, int iteration, CfGraph<SkrInstruction*>* graph) {
                              stageCallback->onCfgCreated(funName, iteration, graph);
                          }
            ).optimize(skrOptimizerConfig);

            stageCallback->onEmitSkrFunc(skrFunc);
        }

        pools->pool1.reset();
        StackFrame stackFrame(pools->pool1);

        if (finalBuildStage >= SparkBuildStage::RVA_Initial) {
            Skr2RvaPseudo::emit(skrFunc, pools->pool1, idGen, symTable, symSize, stackFrame, tempRvas);
            stageCallback->onEmitRva(tempRvas);
        }

        if (finalBuildStage >= SparkBuildStage::RVA_Replaced) {
            RvaPseudoReplacer::replace(tempRvas, stackFrame, symSize);
            stageCallback->onReplaceRvaPseudo(tempRvas);
        }

        if (finalBuildStage >= SparkBuildStage::RVA_Fixed) {
            pools->pool2.reset();
            RvaFixer::fix(tempRvas, rvas, pools->pool2);
            stageCallback->onFixRva(rvas);
        }

        if (finalBuildStage == SparkBuildStage::Bin) {
            assembler.compile(rvas);
        }
    }

    assembler.link();
    auto res = buildResult(symTable, assembler);
    if (finalBuildStage == SparkBuildStage::Bin) {
        stageCallback->onBinary(res);
    }
    return res;
}

PoolsMemoryStats SparkCompiler::getMemoryUsage() {
    return pools->getMemoryUsage();
}

static BuildResult buildResult(SymbolTable& symTable, RvAssembler& assembler) {
    std::unordered_map<StringRef, BuildResult::Function> functions;

    auto publicLabels = assembler.getPublicLabels();
    for (const auto& label : publicLabels) {
        auto* type = symTable.get(label.value);
        if (type->kind == SymbolType::Kind::Function) {
            void* ptr = outBin + label.offset;
            auto fun = BuildResult::Function(
                ptr,
                label.value,
                (SymbolFunctionType*) type
            );
            functions.emplace(label.value, fun);
        }
    }

    return BuildResult(assembler.getSize(), functions);
}
