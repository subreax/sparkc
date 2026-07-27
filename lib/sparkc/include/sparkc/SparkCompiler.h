#pragma once
#include "sparkc/common/StringRef.h"
#include "sparkc/symbol/SymbolType.h"
#include "sparkc/SparkDebugCallback.h"
#include "sparkc/symbol/SymbolTable.h"
#include "sparkc/type/TypeTable.h"
#include "BuildResult.h"
#include "MemUsageStats.h"
#include "SparkRuntime.h"
#include "SparkInitContext.h"
#include <functional>
#include <vector>
#include <unordered_map>

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
    SparkRuntime runtime;
    SparkDebugCallback* debugCallback;
};

class SparkCompiler {
public:
    using OnInitCallback = std::function<void(SparkInitContext&)>;

    static void init(const SparkCompilerConfig& config);
    static void destroy();

    static void addOnInitCallback(OnInitCallback cbk);

    static BuildResult build(const char* src);
    static MemUsageStats getMemoryUsage();
};
