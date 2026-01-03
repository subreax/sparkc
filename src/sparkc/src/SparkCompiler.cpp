#include "sparkc/SparkCompiler.h"

#include "backend/rv/RvaFixer.h"
#include "backend/rv/RvaPseudoReplacer.h"
#include "backend/rv/Skr2RvaPseudo.h"
#include "frontend/lexer/Lexer.h"
#include "frontend/parser/Parser.h"
#include "frontend/semantic/Semantic.h"
#include "skr/SkrEmitter.h"
#include "skr/optimizer/SkrOptimizer.h"
#include "sparkc/SparkCompilerContext.h"

SparkCompiler::SparkCompiler(const SparkCompiler::Initializer& init)
    : pools(init.mem)
    , outBin(init.outBin)
    , outCap(init.outCap)
    , debugCallback(init.debugCallback)
    , skrOptimizerConfig(init.constantFolding, init.deadCodeElim, init.copyPropagation, init.deadStoreElim) {
    reset();
}

SparkCompiler::~SparkCompiler() { delete ctx; }

void SparkCompiler::addStruct(const char* tag, std::initializer_list<StructField> fields) {
    parserTypes.emplace_back(StringRef::cstr(tag));
    ctx->typeTable.declare(StringRef::cstr(tag), fields);
}

void SparkCompiler::addFunction(
    void* ptr,
    const char* name,
    SymbolType* retType,
    std::initializer_list<SymbolType*> params
) {
    ctx->symTable.declareFunc(StringRef::cstr(name), retType, params);
    ctx->assembler.addExternalLabel(StringRef::cstr(name), ptr);
}

SparkBuildInfo SparkCompiler::build(const char* src) {
    try {
        AstProgram* ast = buildAst(pools.pool1, pools.shared, src);
        for (auto* item : ast->items) {
            if (item->kind != AstProgItem::Kind::Function) {
                continue;
            }

            auto* astFunc = (AstFunction*) item;
            auto* skrFunc = ast2skr(astFunc, pools.pool2);
            skrFunc = SkrOptimizer(pools.pool2, skrFunc, debugCallback).optimize(skrOptimizerConfig);
            notifyOptimizeSkrFunc(skrFunc);

            std::vector<RvaInstruction*> rvas;
            skr2rva(skrFunc, pools.pool2, pools.pool3, rvas);

            ctx->assembler.compile(rvas);

            pools.pool2.reset();
            pools.pool3.reset();
        }
        ctx->assembler.link();

        SparkBuildInfo buildInfo(
            pools.getMemoryUsage(),
            ctx->assembler.getPublicLabels(),
            ctx->assembler.getSize(),
            ctx->assembler.getSize() * 100 / outCap
        );
        reset();
        return buildInfo;
    } catch (...) {
        reset();
        std::rethrow_exception(std::current_exception());
    }
}

void SparkCompiler::setDebugCallback(DebugCallback* callback) { this->debugCallback = callback; }

void SparkCompiler::reset() {
    pools.reset();
    parserTypes.clear();
    recreateContext();

    if (debugCallback) {
        debugCallback->setCtx(ctx);
    }
}

void SparkCompiler::recreateContext() {
    delete ctx;
    ctx = new SparkCompilerContext(pools.shared, outBin, outCap);
}

AstProgram* SparkCompiler::buildAst(Allocator& pool, Allocator& sharedPool, const char* src) {
    Lexer lexer(src);
    auto* astProgram = Parser(lexer, pool, sharedPool).parseProgram();
    Semantic(ctx->symTable, ctx->typeTable, ctx->idGen, pool, 1024).process(astProgram);
    notifyAstBuild(astProgram);
    return astProgram;
}

SkrFunction* SparkCompiler::ast2skr(AstFunction* astFunc, Allocator& pool) {
    std::vector<SkrInstruction*> buf;
    SkrFunction* skrFunc
        = SkrEmitter::emit(astFunc, pool, ctx->symTable, ctx->typeTable, ctx->idGen, ctx->labelGen, buf);
    notifyEmitSkrFunc(skrFunc);
    return skrFunc;
}

void SparkCompiler::skr2rva(
    SkrFunction* skrFunc,
    Allocator& skrRvaPool,
    Allocator& tempPool,
    std::vector<RvaInstruction*>& out
) {
    std::vector<RvaInstruction*> buf;
    StackFrame frame(tempPool);
    Skr2RvaPseudo::emit(skrFunc, tempPool, ctx->idGen, ctx->symTable, ctx->symSize, frame, buf);
    notifyEmitRva(buf);
    skrRvaPool.reset(); // now our data in tempPool
    RvaPseudoReplacer::replace(buf, frame, ctx->symSize);
    notifyReplaceRvaPseudo(buf);

    RvaFixer::fix(buf, out, skrRvaPool);
    notifyFixRva(out);
    tempPool.reset(); // now our data in skrRvaPool
}
