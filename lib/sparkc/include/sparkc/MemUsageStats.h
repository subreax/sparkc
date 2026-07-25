#pragma once
#include "sparkc/common/alloc/MemoryUsage.h"

struct MemUsageStats {
    MemUsageStats(
        MemoryUsage shared,
        MemoryUsage pool1,
        MemoryUsage pool2
    )
        : shared(shared)
        , pool1(pool1)
        , pool2(pool2) { }

    MemoryUsage shared;
    MemoryUsage pool1;
    MemoryUsage pool2;
};
