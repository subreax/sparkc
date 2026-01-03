#pragma once
#include "sparkc/backend/rv/RvReg.h"
#include <cstdint>

namespace Rv32Base {
uint32_t rType(uint32_t opcode, uint32_t funct3, uint32_t funct7, RvReg rd, RvReg rs1, RvReg rs2);
uint32_t iType(uint32_t opcode, uint32_t funct3, RvReg rd, RvReg rs1, int32_t imm11);
uint32_t sType(uint32_t opcode, uint32_t funct3, RvReg rs1, RvReg rs2, int32_t imm11);
uint32_t bType(uint32_t opcode, uint32_t funct3, RvReg rs1, RvReg rs2);
uint32_t uType(uint32_t opcode, RvReg rd, int32_t imm);
uint32_t jType(uint32_t opcode, RvReg rd);

uint32_t encodeImmB(int32_t imm12);
uint32_t encodeImmJ(int32_t imm20);

bool isBType(uint32_t instr);
bool isJType(uint32_t instr);
}; // namespace Rv32Base
