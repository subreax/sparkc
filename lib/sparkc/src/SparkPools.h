#pragma once
#include "sparkc/common/alloc/LinearAllocator.h"
#include "sparkc/common/alloc/StatAllocator.h"
#include "sparkc/MemUsageStats.h"

class SparkPools {
public:
    SparkPools(size_t mem)
        : SparkPools(mem / 3, mem / 3, mem / 3) { }

    SparkPools(size_t pool1sz, size_t pool2sz, size_t sharedSz)
        : pool1("pool1", pool1sz)
        , pool2("pool2", pool2sz)
        , shared("shared", sharedSz) { }

    void reset() {
        // clang-format off
        pool1.reset(); pool1.resetPeak();
        pool2.reset(); pool2.resetPeak();
        shared.reset(); shared.resetPeak();
        // clang-format on
    }

    MemUsageStats getMemoryUsage() {
        return MemUsageStats(
            shared.getPeakMemoryUsage(),
            pool1.getPeakMemoryUsage(),
            pool2.getPeakMemoryUsage()
        );
    }

    StatAllocator<LinearAllocator> pool1;
    StatAllocator<LinearAllocator> pool2;
    StatAllocator<LinearAllocator> shared;
};
