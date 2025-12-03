#include "Rv32Base.h"
#include "sparkc/common/Error.h"

static constexpr uint32_t mask3 =  0b111;
static constexpr uint32_t mask5 =  0b11111;
static constexpr uint32_t mask7 =  0b1111111;
static constexpr uint32_t mask12 = 0b111111111111;

static constexpr uint32_t genMask(int bits) {
    int mask = 0;
    while (bits > 0) {
        mask = (mask << 1) | 1;
        --bits;
    }
    return mask;
}

inline constexpr uint32_t regShl(RvReg reg, int shift) {
    return ((uint32_t) reg) << shift;
}

template<int to, int from>
static constexpr uint32_t slice(uint32_t val) {
    return (val >> from) & genMask(to - from + 1);
}

template<int pos>
static constexpr inline uint32_t bit(uint32_t val) {
    return (val >> pos) & 1;
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
    return bit<11>(imm12) << 7
        | slice<4, 1>(imm12) << 8
        | slice<10, 5>(imm12) << 25
        | bit<12>(imm12) << 31;
}

uint32_t Rv32Base::encodeImmJ(int32_t imm20) {
    checkImm20(imm20);
    return (slice<19, 12>(imm20) << 12)
        | (bit<11>(imm20) << 20)
        | (slice<10, 1>(imm20) << 21)
        | (bit<20>(imm20) << 31);
}

bool Rv32Base::isBType(uint32_t instr) {
    return (instr & mask7) == 0b1100011;
}

bool Rv32Base::isJType(uint32_t instr) {
    return (instr & mask7) == 0b1101111;
}