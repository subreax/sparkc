#pragma once
#include <string>
#include <vector>
#include "symbol/SymbolType.h"
#include "SparkPools.h"
#include "SparkCompilerContext.h"
#include "SparkBuildInfo.h"
#include "symbol/SymbolTable.h"
#include "skr/optimizer/SkrOptimizer.h"

class SparkCompiler {
public:
    class DebugCallback : public SkrOptOnCfgCreatedListener {
    public:
        virtual ~DebugCallback() = default;

        virtual void onAstBuild(class AstProgram* ast) { }
        virtual void onEmitSkrFunc(class SkrFunction* skrFunc) { }
        virtual void onOptimizeSkrFunc(class SkrFunction* skrFunc) { }
        virtual void onEmitRva(const std::vector<class RvaInstruction*>& rva) { }
        virtual void onReplaceRvaPseudo(const std::vector<class RvaInstruction*>& rva) { }
        virtual void onFixRva(const std::vector<class RvaInstruction*>& rva) { }

        void setCtx(SparkCompilerContext* ctx) {
            this->ctx = ctx;
        }

    protected:
        const SparkCompilerContext& getCtx() {
            return *ctx;
        }

    private:
        const SparkCompilerContext* ctx;
    };

    struct Initializer {
        size_t mem;
        uint8_t* outBin;
        size_t outCap;

        DebugCallback* debugCallback = nullptr;
        bool constantFolding = true;
        bool deadCodeElim = true;
        bool copyPropagation = true;
        bool deadStoreElim = true;
    };

    SparkCompiler(const Initializer& init);
    ~SparkCompiler();

    void addStruct(const char* tag, std::initializer_list<StructField> fields);
    void addFunction(void* ptr, const char* name, SymbolType* retType, std::initializer_list<SymbolType*> params);
    SparkBuildInfo build(const char* src);

    void setDebugCallback(DebugCallback* callback);

private:
    void reset();
    void recreateContext();
    class AstProgram* buildAst(Allocator& pool, const char* src);
    class SkrFunction* ast2skr(class AstFunction* astFunc, Allocator& pool);
    void skr2rva(class SkrFunction* skrFunc, Allocator& skrOutPool, Allocator& tempPool, std::vector<class RvaInstruction*>& out);

    void notifyAstBuild(class AstProgram* ast) {
        if (debugCallback) {
            debugCallback->onAstBuild(ast);
        }
    }
    void notifyEmitSkrFunc(class SkrFunction* skrFunc) {
        if (debugCallback) {
            debugCallback->onEmitSkrFunc(skrFunc);
        }
    }

    void notifyOptimizeSkrFunc(class SkrFunction* skrFunc) {
        if (debugCallback) {
            debugCallback->onOptimizeSkrFunc(skrFunc);
        }
    }

    void notifyEmitRva(const std::vector<class RvaInstruction*>& rva) {
        if (debugCallback) {
            debugCallback->onEmitRva(rva);
        }
    }

    void notifyReplaceRvaPseudo(const std::vector<class RvaInstruction*>& rva) {
        if (debugCallback) {
            debugCallback->onReplaceRvaPseudo(rva);
        }
    }

    void notifyFixRva(const std::vector<class RvaInstruction*>& rva) {
        if (debugCallback) {
            debugCallback->onFixRva(rva);
        }
    }


    SparkPools pools;
    std::vector<StringRef> parserTypes;
    SkrOptimizer::Config skrOptimizerConfig;

    SparkCompilerContext* ctx = nullptr;

    DebugCallback* debugCallback = nullptr;

    uint8_t *const outBin;
    const size_t outCap;
};

