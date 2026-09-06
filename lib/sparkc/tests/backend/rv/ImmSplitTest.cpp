#include <catch2/catch_test_macros.hpp>
#include <backend/rv/asm/Rv32Base.h>

TEST_CASE("Immutable split test", "[rv-imm]") {
    SECTION("Imm11 hi+ lo+") {
        auto res = Rv32Base::splitImm11(13933);
        REQUIRE(res.hi == 3);
        REQUIRE(res.lo == 1645);
    }

    SECTION("Imm11 hi+ lo-") {
        auto res = Rv32Base::splitImm11(15981);
        REQUIRE(res.hi == 4);
        REQUIRE(res.lo == -403);
    }

    SECTION("Imm11 hi- lo+") {
        auto res = Rv32Base::splitImm11(-14739);
        REQUIRE(res.hi == -4);
        REQUIRE(res.lo == 1645);
    }

    SECTION("Imm11 hi- lo-") {
        auto res = Rv32Base::splitImm11(-12691);
        REQUIRE(res.hi == -3);
        REQUIRE(res.lo == -403);
    }
}