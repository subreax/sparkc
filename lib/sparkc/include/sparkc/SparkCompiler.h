#pragma once
#include "sparkc/common/StringRef.h"
#include "sparkc/symbol/SymbolType.h"
#include "sparkc/SparkDebugCallback.h"
#include "sparkc/symbol/SymbolTable.h"
#include "sparkc/type/TypeTable.h"
#include "MemUsageStats.h"
#include <vector>
#include <unordered_map>

class BuildResult {
public:
    class Function {
    public:
        Function(void* ptr, StringRef name, SymbolFunctionType* type)
            : ptr(ptr), name(name), type(type) {  }

        void* getPointer() const { return ptr; }
        const StringRef& getName() const { return name; }
        SymbolFunctionType* getType() const { return type; }

    private:
        void* ptr;
        StringRef name;
        SymbolFunctionType* type;
    };


    BuildResult() = default;
    BuildResult(size_t binarySize, const std::unordered_map<StringRef, Function>& functions)
        : binarySize(binarySize)
        , functions(functions) { }

    size_t getBinarySize() const {
        return binarySize;
    }

    Function* lookupFunction(StringRef name) {
        auto it = functions.find(name);
        if (it != functions.end()) {
            return &it->second;
        }
        return nullptr;
    }

private:
    size_t binarySize;
    std::unordered_map<StringRef, Function> functions;
};

struct SparkCompilerOptimizations {
    bool constantFolding;
    bool deadCodeElimination;
    bool copyPropagation;
    bool deadStoreElimination;
};

struct SparkCompilerConfig {
    uint8_t* outBin;
    size_t outCap;
    size_t poolSize;
    SparkCompilerOptimizations optimizations;
    SparkDebugCallback* debugCallback;
};

class SparkCompiler {
public:
    static void init(const SparkCompilerConfig& config);
    static void destroy();

    static BuildResult build(const char* src);
    static MemUsageStats getMemoryUsage();

    static const SymbolTable& getSymbolTable();
    static const TypeTable& getTypeTable();
};
