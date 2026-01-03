#include <catch2/catch_test_macros.hpp>
#include <sparkc/backend/rv/asm/RvListing.h>
#include <sparkc/backend/rv/instr/RvaMov.h>

TEST_CASE("Test RvaMov", "[rva]") {
    uint32_t buf[2] = { 0, 0 };
    RvListing listing((uint8_t*) buf, sizeof(buf));
    auto* reg = RvaRegister::get(RvReg::T0);

    SECTION("Mov 50") {
        RvaImm imm(50);
        RvaMov mov(reg, &imm);
        mov.emit(listing);
        REQUIRE(buf[0] == 0x03200293u);
        REQUIRE(buf[1] == 0);
    }

    SECTION("Mov 13854") {
        RvaImm imm(13854);
        RvaMov mov(reg, &imm);
        mov.emit(listing);
        REQUIRE(buf[0] == 0x000032b7u);
        REQUIRE(buf[1] == 0x61e28293u);
    }

    SECTION("Mov -42") {
        RvaImm imm(-42);
        RvaMov mov(reg, &imm);
        mov.emit(listing);
        REQUIRE(buf[0] == 0xfd600293u);
        REQUIRE(buf[1] == 0);
    }

    SECTION("Mov -2050") {
        RvaImm imm(-2050);
        RvaMov mov(reg, &imm);
        mov.emit(listing);
        REQUIRE(buf[0] == 0xfffff2b7u);
        REQUIRE(buf[1] == 0x7fe28293u);
    }

    SECTION("Mov -16385") {
        RvaImm imm(-16385);
        RvaMov mov(reg, &imm);
        mov.emit(listing);
        REQUIRE(buf[0] == 0xffffc2b7u);
        REQUIRE(buf[1] == 0xfff28293u);
    }

    SECTION("Mov -29491") {
        RvaImm imm(-29491);
        RvaMov mov(reg, &imm);
        mov.emit(listing);
        REQUIRE(buf[0] == 0xffff92b7u);
        REQUIRE(buf[1] == 0xccd28293u);
    }
}
