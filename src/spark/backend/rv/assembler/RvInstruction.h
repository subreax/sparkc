#pragma once
#include <cstdint>

class RvInstruction {
public:
    virtual uint32_t build() = 0;
};