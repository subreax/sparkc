#pragma once
#include <cstdint>

class FixedUtils {
public:
    static int32_t fromFloat(float val) {
        return val * 32768.0f;
    }
};