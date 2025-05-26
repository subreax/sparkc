#pragma once
#include <cstdint>
#include <stdexcept>
#include "LinearAllocator.h"

template<typename T>
class BoundArray {
public:
    BoundArray(void* mem, size_t itemsCount) : mem((T*) mem), itemsCount(itemsCount) {  }

    static BoundArray<T> create(size_t itemsCount, LinearAllocator& allocator) {
        auto* mem = allocator.allocate(itemsCount * sizeof(T));
        return BoundArray<T>(mem, itemsCount);
    }

    T& operator[](size_t idx) {
        if (idx < itemsCount) {
            return mem[idx];
        }
        throw std::out_of_range("BoundArray index is out of range");
    }

    const T& operator[](size_t idx) const {
        if (idx < itemsCount) {
            return mem[idx];
        }
        throw std::out_of_range("BoundArray index is out of range");
    }

    size_t size() const { return itemsCount; }

private:
    T* mem;
    size_t itemsCount;
};