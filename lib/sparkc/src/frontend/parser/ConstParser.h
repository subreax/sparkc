#pragma once
#include "sparkc/common/StringRef.h"
#include <cstdint>

class ConstParser {
public:
    static bool parseInt(StringRef value, int32_t& out);
    static bool parseFloat(StringRef value, float& out);
};