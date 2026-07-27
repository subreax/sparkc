#pragma once
#include "sparkc/common/alloc/MemoryStats.h"

struct PoolsMemoryStats {
    PoolsMemoryStats(
        MemoryStats shared,
        MemoryStats pool1,
        MemoryStats pool2
    )
        : shared(shared)
        , pool1(pool1)
        , pool2(pool2) { }

    MemoryStats shared;
    MemoryStats pool1;
    MemoryStats pool2;
};
