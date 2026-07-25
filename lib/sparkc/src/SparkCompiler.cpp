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

static SparkPools* pools = nullptr;
static uint8_t* outBin = nullptr;
static size_t outCap = 0;

static SparkDebugCallback nullDebugCallback;
static SparkDebugCallback* debugCallback = &nullDebugCallback;

static SymbolTable* symTable = nullptr;
static TypeTable* typeTable = nullptr;

static SparkRuntime runtime;
static SkrOptimizerConfig skrOptimizerConfig;

static BuildResult buildResult(RvAssembler& assembler);

void SparkCompiler::init(const SparkCompilerConfig& config) {
    if (pools != nullptr) {
        sparkError("SparkCompiler", "SparkCompiler already initialized");
    }

    pools = new SparkPools(config.poolSize);
    outBin = config.outBin;
    outCap = config.outCap;
    runtime = config.runtime;
    debugCallback = config.debugCallback;
    skrOptimizerConfig.constantFolding = config.optimizations.constantFolding;
    skrOptimizerConfig.copyPropagation = config.optimizations.copyPropagation;
    skrOptimizerConfig.deadCodeElimination = config.optimizations.deadCodeElimination;
    skrOptimizerConfig.deadStoreElimination = config.optimizations.deadStoreElimination;
}

void SparkCompiler::destroy() {
}

BuildResult SparkCompiler::build(const char* src) {
    delete symTable;
    delete typeTable;
    pools->reset();

    symTable = new SymbolTable(pools->shared);
    typeTable = new TypeTable(pools->shared);
    SymbolSize symSize(*symTable, *typeTable);
    IdentifierGen idGen(pools->shared);
    LabelGen labelGen(pools->shared);
    RvAssembler assembler(outBin, outCap);

    assembler.addExternalLabel(StringRef::cstr(SparkRuntime::divq15FunName), (void*) runtime.divq15);

    AstFactory astFactory(pools->pool1);
    Frontend frontend(src, astFactory, *symTable, *typeTable, idGen);

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
            *symTable,
            *typeTable,
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
        Skr2RvaPseudo::emit(skrFunc, pools->pool1, idGen, *symTable, symSize, stackFrame, tempRvas);
        debugCallback->onEmitRva(tempRvas);

        RvaPseudoReplacer::replace(tempRvas, stackFrame, symSize);
        debugCallback->onReplaceRvaPseudo(tempRvas);

        pools->pool2.reset();
        RvaFixer::fix(tempRvas, rvas, pools->pool2);
        debugCallback->onFixRva(rvas);

        assembler.compile(rvas);
    }

    assembler.link();
    return buildResult(assembler);
}

MemUsageStats SparkCompiler::getMemoryUsage() {
    return pools->getMemoryUsage();
}

const SymbolTable& SparkCompiler::getSymbolTable() {
    return *symTable;
}

const TypeTable& SparkCompiler::getTypeTable() {
    return *typeTable;
}

static BuildResult buildResult(RvAssembler& assembler) {
    std::unordered_map<StringRef, BuildResult::Function> functions;

    auto publicLabels = assembler.getPublicLabels();
    for (const auto& label : publicLabels) {
        auto* type = symTable->get(label.value);
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
