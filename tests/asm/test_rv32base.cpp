#include "backend/rv/asm/Rv32Base.h"
#include <catch2/catch_test_macros.hpp>

constexpr uint32_t _0101 = 0b01010101010101010101010101010101u;
constexpr uint32_t _1010 = 0b10101010101010101010101010101010u;

// reg 01010 - A0
// reg 10101 - S5

TEST_CASE("Test core risc-v instructions", "[riscv-core]") {
    // R Type
    // ...0101
    // f7: 0101010 rs2:10101 rs1:01010 f3:101 rd:01010 op:1010101
    REQUIRE ( Rv32Base::rType(0b1010101, 0b101, 0b0101010, RvReg::A0, RvReg::A0, RvReg::S5) == _0101 );

    // ...1010
    // f7:1010101 rs2:01010 rs1:10101 f3:010 rd:10101 op:0101010
    REQUIRE ( Rv32Base::rType(0b0101010, 0b010, 0b1010101, RvReg::S5, RvReg::S5, RvReg::A0) == _1010 );


    // I Type
    // ...0101
    // imm:010101010101 rs1:01010 f3:101 rd:01010 op:1010101
    REQUIRE( Rv32Base::iType(0b1010101, 0b101, RvReg::A0, RvReg::A0, 0b010101010101) == _0101 );

    // ...0101
    // imm:101010101010 rs1:10101 f3:010 rd:10101 op:0101010
    // 0b101010101010
    REQUIRE( Rv32Base::iType(0b0101010, 0b010, RvReg::S5, RvReg::S5, -1366) == _1010 );


    // S Type
    // rs2:10101 rs1:01010 f3:101 op:1010101
    // imm: 010101001010
    REQUIRE ( Rv32Base::sType(0b1010101, 0b101, RvReg::A0, RvReg::S5, 0b010101001010) == _0101 );

    // rs2:01010 rs1:10101 f3:010 op:0101010
    // imm: 101010110101
    REQUIRE( Rv32Base::sType(0b0101010, 0b010, RvReg::S5, RvReg::A0, -1355) == _1010 );


    // J Type
    // imm:01010101010101010101 rd:01010 op:1010101
    REQUIRE( Rv32Base::jType(0b1010101, RvReg::A0, 0b001010101110101010100) == _0101 );

    // imm:10101010101010101010 rd:10101 op:0101010
    // 2^21 - 0b110101010001010101010
    REQUIRE( Rv32Base::jType(0b0101010, RvReg::S5, -351574) == _1010 );

    // 1 0000000000 1 00000000 11111 0000000
    REQUIRE( Rv32Base::jType(0, (RvReg) 0b11111, -1046528) == 0b10000000000100000000111110000000u );

    // 0 1111111111 0 11111111 00000 1111111
    REQUIRE( Rv32Base::jType(0b1111111, RvReg::ZERO, 0b011111111011111111110u) == 0b01111111111011111111000001111111u );
}
