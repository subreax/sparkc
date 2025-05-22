#include "Rv32Base.h"
#include <stdexcept>

constexpr uint32_t _rv_mask3 =  0b111;
constexpr uint32_t _rv_mask5 =  0b11111;
constexpr uint32_t _rv_mask7 =  0b1111111;
constexpr uint32_t _rv_mask12 = 0b111111111111;

constexpr uint32_t _rv_mask(int bits) {
    int mask = 0;
    while (bits > 0) {
        mask = (mask << 1) | 1;
        --bits;
    }
    return mask;
}

inline constexpr uint32_t _rv_reg_shl(RvReg reg, int shift) {
    return ((uint32_t) reg) << shift;
}

template<int to, int from>
constexpr uint32_t _rv_slice(uint32_t val) {
    return (val >> from) & _rv_mask(to - from + 1);
}

constexpr uint32_t _rv_bit(uint32_t val, int pos) {
    return (val >> pos) & 1;
}

void _rv_dbgCheckImm11(int32_t imm11) {
    if (imm11 < -2048 || imm11 > 2047) {
        printf("Imm11 is out of range: %d\n", imm11);
        std::abort();
    }
}


uint32_t Rv32Base::rType(uint32_t opcode, uint32_t funct3, uint32_t funct7, RvReg rd, RvReg rs1, RvReg rs2) {
    return (opcode & _rv_mask7) 
        | _rv_reg_shl(rd, 7)
        | ((funct3 & _rv_mask3) << 12)
        | _rv_reg_shl(rs1, 15)
        | _rv_reg_shl(rs2, 20)
        | ((funct7 & _rv_mask7) << 25);
}

uint32_t Rv32Base::iType(uint32_t opcode, uint32_t funct3, RvReg rd, RvReg rs1, int32_t imm11) {
    _rv_dbgCheckImm11(imm11);
    return (opcode & _rv_mask7)
        | _rv_reg_shl(rd, 7)
        | ((funct3 & _rv_mask3) << 12)
        | _rv_reg_shl(rs1, 15)
        | (imm11 & _rv_mask12) << 20;
}

uint32_t Rv32Base::sType(uint32_t opcode, uint32_t funct3, RvReg rs1, RvReg rs2, int32_t imm11) {
    _rv_dbgCheckImm11(imm11);
    return (opcode & _rv_mask7)
        | ((imm11 & _rv_mask5) << 7)
        | ((funct3 & _rv_mask3) << 12)
        | _rv_reg_shl(rs1, 15)
        | _rv_reg_shl(rs2, 20)
        | ((imm11 >> 5) << 25);
}

uint32_t Rv32Base::bType(uint32_t opcode, uint32_t funct3, RvReg rs1, RvReg rs2) {
    return (opcode & _rv_mask7)
        | (funct3 & _rv_mask3) << 12
        | _rv_reg_shl(rs1, 15)
        | _rv_reg_shl(rs2, 20);
}

uint32_t Rv32Base::jType(uint32_t opcode, RvReg rd) {
    return (opcode & _rv_mask7) | _rv_reg_shl(rd, 7);
}

uint32_t Rv32Base::encodeImmB(int32_t imm12) {
    // todo: check imm12
    return _rv_bit(imm12, 11) << 7
        | _rv_slice<4, 1>(imm12) << 8
        | _rv_slice<10, 5>(imm12) << 25
        | _rv_bit(imm12, 12) << 31;
}

uint32_t Rv32Base::encodeImmJ(int32_t imm20) {
    // todo: check imm20
    return (_rv_slice<19, 12>(imm20) << 12)
        | (_rv_bit(imm20, 11) << 20)
        | (_rv_slice<10, 1>(imm20) << 21)
        | (_rv_bit(imm20, 20) << 31);
}

bool Rv32Base::isBType(uint32_t instr) {
    return (instr & _rv_mask7) == 0b1100011;
}

bool Rv32Base::isJType(uint32_t instr) {
    return (instr & _rv_mask7) == 0b1101111;
}
