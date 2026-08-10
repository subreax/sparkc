#pragma once
#include <functional>
#include "sparkc/symbol/SymbolType.h"
#include "sparkc/symbol/SymbolTable.h"
#include "sparkc/type/TypeTable.h"
#include "sparkc/SparkStageCallback.h"
#include "BuildResult.h"
#include "PoolsMemoryStats.h"
#include "SparkRuntime.h"
#include "SparkInitContext.h"
#include "SparkBuildStage.h"
#include "SparkOptimization.h"

struct SparkCompilerConfig {
    uint8_t* outBin;
    size_t outCap;
    size_t poolSize;
    uint32_t optimizations;
    SparkRuntime runtime;
    SparkBuildStage finalBuildStage;
    SparkStageCallback* stageCallback;
};

class SparkCompiler {
public:
    using OnInitCallback = std::function<void(SparkInitContext&)>;

    static void init(const SparkCompilerConfig& config);
    static void destroy();

    static void addOnInitCallback(OnInitCallback cbk);

    static BuildResult build(const char* src);
    static PoolsMemoryStats getMemoryUsage();
};
