#include "sparkc/SparkCompiler.h"
#include "SparkPools.h"

#include "sparkc/frontend/Frontend.h"
#include "skr/SkrEmitter.h"
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

static SparkDebugCallback nullDebugCallback;
static SparkDebugCallback* debugCallback = &nullDebugCallback;

static SparkRuntime runtime;
static SkrOptimizerConfig skrOptimizerConfig;

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
    debugCallback = config.debugCallback;
    skrOptimizerConfig.constantFolding = config.optimizations & SPARK_OPT_CONSTANT_FOLDING;
    skrOptimizerConfig.copyPropagation = config.optimizations & SPARK_OPT_COPY_PROPAGATION;
    skrOptimizerConfig.deadCodeElimination = config.optimizations & SPARK_OPT_DEAD_CODE_ELIM;
    skrOptimizerConfig.deadStoreElimination = config.optimizations & SPARK_OPT_DEAD_STORE_ELIM;
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

    debugCallback->setSymbolTable(symTable);
    assembler.addExternalLabel(StringRef::cstr(SparkRuntime::divq15FunName), (void*) runtime.divq15);
    {
        SparkInitContextImpl initCtx(idGen, symTable, typeTable, assembler);
        for (auto& cbk : initCallbacks) {
            cbk(initCtx);
        }
    }

    AstFactory astFactory(pools->pool1);
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
        debugCallback->onAstBuild(astProgItem);

        if (astProgItem->kind != AstProgItem::Kind::Function) {
            continue;
        }

        auto* skrFunc = SkrEmitter::emit(
            (AstFunction*) astProgItem,
            pools->pool2,
            symTable,
            typeTable,
            idGen,
            labelGen,
            skrsBuf
        );
        debugCallback->onEmitSkrFunc(skrFunc);

        skrFunc = SkrOptimizer(
                      pools->pool2,
                      skrFunc,
                      [&](StringRef funName, int iteration, CfGraph<SkrInstruction*>* graph) {
                          debugCallback->onCfgCreated(funName, iteration, graph);
                      }
        ).optimize(skrOptimizerConfig);
        debugCallback->onOptimizeSkrFunc(skrFunc);

        pools->pool1.reset();
        StackFrame stackFrame(pools->pool1);
        Skr2RvaPseudo::emit(skrFunc, pools->pool1, idGen, symTable, symSize, stackFrame, tempRvas);
        debugCallback->onEmitRva(tempRvas);

        RvaPseudoReplacer::replace(tempRvas, stackFrame, symSize);
        debugCallback->onReplaceRvaPseudo(tempRvas);

        pools->pool2.reset();
        RvaFixer::fix(tempRvas, rvas, pools->pool2);
        debugCallback->onFixRva(rvas);

        assembler.compile(rvas);
    }

    assembler.link();
    return buildResult(symTable, assembler);
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
