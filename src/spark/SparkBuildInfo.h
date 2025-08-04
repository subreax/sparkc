#pragma once
#include <vector>
#include "SparkMemoryPercentUsage.h"
#include "common/StringRef.h"

struct SparkBuildInfo {
    SparkBuildInfo() = default;

    SparkBuildInfo(
        const SparkMemoryPercentUsage& memUsage, 
        const std::vector<RvListing::Label>& publicLabelsOffsets, 
        size_t binarySize
    )
        : memoryUsage(memUsage)
        , publicLabelsOffsets(publicLabelsOffsets)
        , binarySize(binarySize) {  }

    SparkMemoryPercentUsage memoryUsage;

    // todo: replace vector with unordered_map
    std::vector<RvListing::Label> publicLabelsOffsets;
    size_t binarySize;
};

