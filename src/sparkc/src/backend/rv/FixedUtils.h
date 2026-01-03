#pragma once
#include <cmath>
#include <cstdint>

class FixedUtils {
public:
    static int32_t fromFloat(float val) {
        return roundf(val * 32768.0f);
    }
};