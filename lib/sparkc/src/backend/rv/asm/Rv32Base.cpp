#include "Rv32Base.h"
#include "sparkc/common/Error.h"
#include "BinUtils.h"

static constexpr uint32_t mask3 = BinUtils::mask<3>();
static constexpr uint32_t mask5 = BinUtils::mask<5>();
static constexpr uint32_t mask7 = BinUtils::mask<7>();
static constexpr uint32_t mask12 = BinUtils::mask<12>();

inline constexpr uint32_t regShl(RvReg reg, int shift) {
    return ((uint32_t) reg) << shift;
}

static void checkImm(int32_t imm, int32_t minVal, int32_t maxVal) {
    if (imm < minVal || imm > maxVal) {
        sparkError("Rv32Base", "Imm is out of range [%d; %d]: %d", minVal, maxVal, imm);
    }
}

static void checkImm11(int32_t imm11) {
    checkImm(imm11, -2048, 2047);
}

static void checkImm12(int32_t imm12) {
    checkImm(imm12, -4096, 4095);
}

static void checkImm20(int32_t imm20) {
    checkImm(imm20, -1048576, 1048575);
}

uint32_t Rv32Base::rType(uint32_t opcode, uint32_t funct3, uint32_t funct7, RvReg rd, RvReg rs1, RvReg rs2) {
    return (opcode & mask7)
        | regShl(rd, 7)
        | ((funct3 & mask3) << 12)
        | regShl(rs1, 15)
        | regShl(rs2, 20)
        | ((funct7 & mask7) << 25);
}

uint32_t Rv32Base::iType(uint32_t opcode, uint32_t funct3, RvReg rd, RvReg rs1, int32_t imm11) {
    checkImm11(imm11);
    return (opcode & mask7)
        | regShl(rd, 7)
        | ((funct3 & mask3) << 12)
        | regShl(rs1, 15)
        | (imm11 & mask12) << 20;
}

uint32_t Rv32Base::sType(uint32_t opcode, uint32_t funct3, RvReg rs1, RvReg rs2, int32_t imm11) {
    checkImm11(imm11);
    return (opcode & mask7)
        | ((imm11 & mask5) << 7)
        | ((funct3 & mask3) << 12)
        | regShl(rs1, 15)
        | regShl(rs2, 20)
        | ((imm11 >> 5) << 25);
}

uint32_t Rv32Base::bType(uint32_t opcode, uint32_t funct3, RvReg rs1, RvReg rs2) {
    return (opcode & mask7)
        | (funct3 & mask3) << 12
        | regShl(rs1, 15)
        | regShl(rs2, 20);
}

uint32_t Rv32Base::uType(uint32_t opcode, RvReg rd, int32_t imm) {
    return (opcode & mask7)
        | regShl(rd, 7)
        | imm << 12;
}

uint32_t Rv32Base::jType(uint32_t opcode, RvReg rd) {
    return (opcode & mask7) | regShl(rd, 7);
}

uint32_t Rv32Base::encodeImmB(int32_t imm12) {
    checkImm12(imm12);
    return BinUtils::bit<11>(imm12) << 7
        | BinUtils::slice<4, 1>(imm12) << 8
        | BinUtils::slice<10, 5>(imm12) << 25
        | BinUtils::bit<12>(imm12) << 31;
}

uint32_t Rv32Base::encodeImmJ(int32_t imm20) {
    checkImm20(imm20);
    return (BinUtils::slice<19, 12>(imm20) << 12)
        | (BinUtils::bit<11>(imm20) << 20)
        | (BinUtils::slice<10, 1>(imm20) << 21)
        | (BinUtils::bit<20>(imm20) << 31);
}

uint32_t Rv32Base::encodeImmU(int32_t imm20) {
    checkImm20(imm20);
    return imm20 << 12;
}

uint32_t Rv32Base::readOpcode(uint32_t instr) {
    return instr & mask7;
}

uint32_t Rv32Base::iTypeReadFunct3(uint32_t instr) {
    return BinUtils::slice<14, 12>(instr);
}

RvReg Rv32Base::uTypeReadRd(uint32_t instr) {
    return static_cast<RvReg>(BinUtils::slice<11, 7>(instr));
}

bool Rv32Base::isImm11(int32_t imm) {
    return imm >= -2048 && imm <= 2047;
}

bool Rv32Base::isImm20(int32_t imm) {
    return imm >= -1048576 && imm <= 1048575;
}

Rv32Base::BinSplit Rv32Base::splitImm11(int32_t imm) {
    BinSplit res;
    res.hi = BinUtils::hi<12>(imm);
    res.lo = BinUtils::lo<12>(imm);
    if (res.lo < 0) {
        res.hi += 1;
    }
    return res;
}
