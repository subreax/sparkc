#pragma once
#include <cstdint>

class BinUtils {
public:
    template <int bits>
    static constexpr uint32_t mask() {
        int count = bits;
        uint32_t mask = 0;
        while (count > 0) {
            mask = (mask << 1) | 1;
            count--;
        }
        return mask;
    }

    template <int bits>
    static constexpr int32_t sext(int32_t imm) {
        return (imm << (32 - bits)) >> (32 - bits);
    }

    template <int bits>
    static constexpr int32_t lo(int32_t imm) {
        return sext<bits>(imm & mask<bits>());
    }

    template <int bits>
    static constexpr int32_t hi(int32_t imm) {
        constexpr auto loBits = 32 - bits;
        return (imm + (1 << (loBits - 1))) >> loBits;
    }

    template <int to, int from>
    static constexpr uint32_t slice(uint32_t val) {
        return (val >> from) & mask<to - from + 1>();
    }

    template <int pos>
    static constexpr inline uint32_t bit(uint32_t val) {
        return (val >> pos) & 1;
    }
};
