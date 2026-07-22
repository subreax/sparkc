#pragma once
#include <cstdint>
#include "sparkc/common/StringRef.h"

struct SparkRuntime {
    static constexpr const char* divq15FunName = "divq15";

    int32_t (*divq15)(int32_t a, int32_t b) = nullptr;
};