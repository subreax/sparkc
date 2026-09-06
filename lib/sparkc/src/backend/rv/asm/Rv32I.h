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

    static uint32_t or_(RvReg rd, RvReg rs1, RvReg rs2) {
        return Rv32Base::rType(0b0110011, 0x6, 0, rd, rs1, rs2);
    }

    static uint32_t ori(RvReg rd, RvReg rs1, uint32_t imm5) {
        return Rv32Base::iType(0b0010011, 0x6, rd, rs1, imm5 & 0b11111);
    }

    static uint32_t slli(RvReg rd, RvReg rs1, uint32_t imm5) {
        return Rv32Base::iType(0b0010011, 0x1, rd, rs1, imm5 & 0b11111);
    }

    static uint32_t srli(RvReg rd, RvReg rs1, uint32_t imm5) {
        return Rv32Base::iType(0b0010011, 0x5, rd, rs1, imm5 & 0b11111);
    }

    static uint32_t slt(RvReg rd, RvReg rs1, RvReg rs2) {
        return Rv32Base::rType(0b0110011, 0x2, 0, rd, rs1, rs2);
    }

    static uint32_t sltu(RvReg rd, RvReg rs1, RvReg rs2) {
        return Rv32Base::rType(0b0110011, 0x3, 0, rd, rs1, rs2);
    }

    static uint32_t slti(RvReg rd, RvReg rs1, int32_t imm11) {
        return Rv32Base::iType(0b0010011, 0x2, rd, rs1, imm11);
    }

    static uint32_t sltiu(RvReg rd, RvReg rs1, uint32_t imm11) {
        return Rv32Base::iType(0b0010011, 0x3, rd, rs1, imm11);
    }

    static uint32_t lw(RvReg rd, RvReg rs1, int32_t imm11) {
        return Rv32Base::iType(0b11, 0x2, rd, rs1, imm11);
    }

    static uint32_t sw(RvReg rs1, int32_t imm11, RvReg rs2) {
        return Rv32Base::sType(0b0100011, 0x2, rs1, rs2, imm11);
    }

    static uint32_t jal(RvReg rd) {
        return Rv32Base::jType(JalOpcode, rd);
    }

    static uint32_t jal(RvReg rd, uint32_t imm) {
        return Rv32Base::jType(JalOpcode, rd) | Rv32Base::encodeImmJ(imm);
    }

    static uint32_t jalr(RvReg rd, RvReg rs1, int32_t imm11) {
        return Rv32Base::iType(JalrOpcode, 0, rd, rs1, imm11);
    }

    static uint32_t beq(RvReg rs1, RvReg rs2) {
        return Rv32Base::bType(BranchOpcode, 0, rs1, rs2);
    }

    static uint32_t bne(RvReg rs1, RvReg rs2) {
        return Rv32Base::bType(BranchOpcode, 1, rs1, rs2);
    }

    static uint32_t blt(RvReg rs1, RvReg rs2) {
        return Rv32Base::bType(BranchOpcode, 0x4, rs1, rs2);
    }

    static uint32_t bge(RvReg rs1, RvReg rs2) {
        return Rv32Base::bType(BranchOpcode, 0x5, rs1, rs2);
    }

    static uint32_t lui(RvReg rd, int32_t imm) {
        return Rv32Base::uType(0b0110111, rd, imm);
    }

    static uint32_t auipc(RvReg rd, int32_t imm) {
        return Rv32Base::uType(AuipcOpcode, rd, imm);
    }

    // pseudo

    static uint32_t ret() {
        return Rv32I::jalr(RvReg::ZERO, RvReg::RA, 0);
    }

    static uint32_t seqz(RvReg rd, RvReg rs) {
        return sltiu(rd, rs, 1);
    }

    static uint32_t snez(RvReg rd, RvReg rs) {
        return sltu(rd, RvReg::ZERO, rs);
    }

    static uint32_t sltz(RvReg rd, RvReg rs) {
        return slt(rd, rs, RvReg::ZERO);
    }

    static uint32_t sgtz(RvReg rd, RvReg rs) {
        return slt(rd, RvReg::ZERO, rs);
    }

    static uint32_t nop() {
        return addi(RvReg::ZERO, RvReg::ZERO, 0);
    }

    // set greater than
    static uint32_t sgt(RvReg rd, RvReg rs1, RvReg rs2) {
        return slt(rd, rs2, rs1);
    }

    // checkers

    static bool isJal(uint32_t instr) {
        return Rv32Base::readOpcode(instr) == JalOpcode;
    }

    static bool isJalr(uint32_t instr) {
        return Rv32Base::readOpcode(instr) == JalrOpcode && Rv32Base::iTypeReadFunct3(instr) == 0;
    }

    static bool isAuipc(uint32_t instr) {
        return Rv32Base::readOpcode(instr) == AuipcOpcode;
    }

    static bool isBranch(uint32_t instr) {
        return Rv32Base::readOpcode(instr) == BranchOpcode;
    }

    static constexpr uint32_t JalOpcode = 0b1101111u;
    static constexpr uint32_t JalrOpcode = 0b1100111u;
    static constexpr uint32_t BranchOpcode = 0b1100011u;
    static constexpr uint32_t AuipcOpcode = 0b0010111u;
};