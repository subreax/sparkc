#pragma once
#include <cstdint>
#include <cmath>

class FixedUtils {
public:
    static int32_t fromFloat(float val) {
        return roundf(val * 32768.0f);
    }
};