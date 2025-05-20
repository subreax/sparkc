#pragma once
#include "Rv32Base.h"

class Rv32M {
public:
    static uint32_t mul(RvReg rd, RvReg rs1, RvReg rs2) {
        return Rv32Base::rType(0b0110011, 0, 0x01, rd, rs1, rs2);
    }

    static uint32_t div(RvReg rd, RvReg rs1, RvReg rs2) {
        return Rv32Base::rType(0b0110011, 0x4, 0x01, rd, rs1, rs2);
    }

    static uint32_t rem(RvReg rd, RvReg rs1, RvReg rs2) {
        return Rv32Base::rType(0b0110011, 0x6, 0x01, rd, rs1, rs2);
    }
};