#pragma once
#include "Rv32Base.h"

class Rv32I {
public:
    static uint32_t addi(RvReg rd, RvReg rs1, int32_t imm11) {
        return Rv32Base::iType(0b0010011, 0, rd, rs1, imm11);
    }

    static uint32_t add(RvReg rd, RvReg rs1, RvReg rs2) {
        return Rv32Base::rType(0b0110011, 0, 0, rd, rs1, rs2);
    }

    static uint32_t sub(RvReg rd, RvReg rs1, RvReg rs2) {
        return Rv32Base::rType(0b0110011, 0, 0x20, rd, rs1, rs2);
    }

    static uint32_t lw(RvReg rd, RvReg rs1, int32_t imm11) {
        return Rv32Base::iType(0b11, 0x2, rd, rs1, imm11);
    }

    static uint32_t sw(RvReg rs1, int32_t imm11, RvReg rs2) {
        return Rv32Base::sType(0b0100011, 0x2, rs1, rs2, imm11);
    }

    static uint32_t jalr(RvReg rd, RvReg rs1, int32_t imm11) {
        return Rv32Base::iType(0b1100111, 0, rd, rs1, imm11);
    }
};