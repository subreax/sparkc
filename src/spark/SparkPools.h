#pragma once
#include "common/alloc/LinearAllocator.h"
#include "common/alloc/StatAllocator.h"
#include "SparkMemoryPercentUsage.h"

class SparkPools {
public:
    SparkPools(size_t mem) : SparkPools(mem / 4, mem / 4, mem / 4, mem / 4) {  }

    SparkPools(size_t pool1sz, size_t pool2sz, size_t pool3sz, size_t sharedSz)
        : pool1("pool1", pool1sz)
        , pool2("pool2", pool2sz) 
        , pool3("pool3", pool3sz)
        , shared("shared", sharedSz) { }

    void reset() {
        pool1.reset(); pool1.resetPeak();
        pool2.reset(); pool2.resetPeak();
        pool3.reset(); pool3.resetPeak();
        shared.reset(); shared.resetPeak();
    }

    SparkMemoryPercentUsage getMemoryUsage() const {
        return SparkMemoryPercentUsage(
            pool1.getPeakUsageInPercent(), 
            pool2.getPeakUsageInPercent(), 
            pool3.getPeakUsageInPercent(), 
            shared.getPeakUsageInPercent()
        );
    }

    StatAllocator<LinearAllocator> pool1;
    StatAllocator<LinearAllocator> pool2;
    StatAllocator<LinearAllocator> pool3;
    StatAllocator<LinearAllocator> shared;
};

