#pragma once
#include <cstdint>
#include "../OutOfBoundsException.h"

class BitArray {
public:
    BitArray(size_t bitsCount)
        : data(allocData(bitsCount))
        , bitsCount(bitsCount) {  }

    ~BitArray() {
        delete[] data;
    }

    bool get(size_t bit) const {
        if (bit >= bitsCount) {
            throw OutOfBoundsException(0, bitsCount, bit);
        }

        size_t idx = bit / 32;
        size_t bitPos = bit % 32;
        return ((data[idx] >> bitPos) & 1) != 0;
    }

    void set(size_t bit) {
        if (bit >= bitsCount) {
            throw OutOfBoundsException(0, bitsCount, bit);
        }

        size_t idx = bit / 32;
        size_t bitPos = bit % 32;
        data[idx] = data[idx] | (1 << bit);
    }

    void reset(size_t bit) {
        if (bit >= bitsCount) {
            throw OutOfBoundsException(0, bitsCount, bit);
        }

        size_t idx = bit / 32;
        size_t bitPos = bit % 32;
        data[idx] = data[idx] & ~(1 << bit);
    }

    bool operator[](size_t idx) const {
        return get(idx);
    }

private:
    static uint32_t* allocData(size_t bitsCount) {
        size_t itemsCount = (bitsCount + 31) / 32;
        auto* data = new uint32_t[itemsCount];
        for (size_t i = 0; i < itemsCount; i++) {
            data[i] = 0;
        }
        return data;
    }

    static uint32_t _reset(uint32_t item, size_t bit) {
        return item & ~(1 << bit);
    }

    static uint32_t _set(uint32_t item, size_t bit, bool enabled) {
        return item | (((int) enabled) << bit);
    }

    size_t indexOf(size_t bit) {
        return bit / 32;
    }



    uint32_t* data;
    const size_t bitsCount;
};
