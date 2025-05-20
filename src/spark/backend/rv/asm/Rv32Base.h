#pragma once
#include <cstdint>
#include "../RvReg.h"

namespace Rv32Base {
uint32_t rType(uint32_t opcode, uint32_t funct3, uint32_t funct7, RvReg rd, RvReg rs1, RvReg rs2);
uint32_t iType(uint32_t opcode, uint32_t funct3, RvReg rd, RvReg rs1, int32_t imm11);
uint32_t sType(uint32_t opcode, uint32_t funct3, RvReg rs1, RvReg rs2, int32_t imm11);
uint32_t jType(uint32_t opcode, RvReg rd, int32_t imm20);
};
