#pragma once
#include <vector>
#include "SparkMemoryPercentUsage.h"
#include "common/StringRef.h"
#include "common/Error.h"
#include "backend/rv/asm/RvListing.h"

struct SparkBuildInfo {
    SparkBuildInfo() = default;

    SparkBuildInfo(
        const SparkMemoryPercentUsage& memUsage, 
        const std::vector<RvListing::Label>& publicLabelsOffsets, 
        size_t binarySize,
        size_t binarySizeUsage
    )
        : memoryUsage(memUsage)
        , publicLabelsOffsets(publicLabelsOffsets)
        , binarySizeUsage(binarySizeUsage) {  }

    size_t findLabel(const char* name) {
        auto nameRef = StringRef::cstr(name);
        for (const auto& label : publicLabelsOffsets) {
            if (label.value == nameRef) {
                return label.offset;
            }
        }
        sparkError("SparkBuildInfo", "findLabel failed for '%s'", name);
        return 0;
    }

    SparkMemoryPercentUsage memoryUsage;

    // todo: replace vector with unordered_map
    std::vector<RvListing::Label> publicLabelsOffsets;
    size_t binarySize;
    size_t binarySizeUsage;
};

