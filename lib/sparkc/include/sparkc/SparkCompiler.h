#pragma once
#include <functional>
#include "sparkc/symbol/SymbolType.h"
#include "sparkc/symbol/SymbolTable.h"
#include "sparkc/type/TypeTable.h"
#include "sparkc/SparkDebugCallback.h"
#include "BuildResult.h"
#include "PoolsMemoryStats.h"
#include "SparkRuntime.h"
#include "SparkInitContext.h"

enum SparkOptimization {
    SPARK_OPT_CONSTANT_FOLDING = 1,
    SPARK_OPT_DEAD_CODE_ELIM = 2,
    SPARK_OPT_COPY_PROPAGATION = 4,
    SPARK_OPT_DEAD_STORE_ELIM = 8,
    SPARK_OPT_ALL = 0xffffffffu
};

struct SparkCompilerConfig {
    uint8_t* outBin;
    size_t outCap;
    size_t poolSize;
    uint32_t optimizations;
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
    static PoolsMemoryStats getMemoryUsage();
};
