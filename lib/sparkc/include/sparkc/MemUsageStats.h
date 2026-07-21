#pragma once
#include "sparkc/common/alloc/MemoryUsage.h"

struct MemUsageStats {
    MemUsageStats(
        MemoryUsage shared,
        MemoryUsage pool1,
        MemoryUsage pool2,
        MemoryUsage pool3
    )
    : shared(shared)
    , pool1(pool1)
    , pool2(pool2)
    , pool3(pool3) {  }

    MemoryUsage shared;
    MemoryUsage pool1;
    MemoryUsage pool2;
    MemoryUsage pool3;
};
