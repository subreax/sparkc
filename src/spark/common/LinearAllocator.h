#pragma once
#include <memory>
#include "NoMemoryException.h"

class LinearAllocator {
public:
    LinearAllocator(size_t sz, bool zeroMem = false) {
        begin = (uint8_t*) malloc(sz);
        if (zeroMem) {
            memset(begin, 0, sz);
        }
        ptr = begin;
        end = begin + sz;
    }

    LinearAllocator(const LinearAllocator&) = delete;
    LinearAllocator& operator=(const LinearAllocator&) = delete;
    LinearAllocator(LinearAllocator&&) = delete;

    ~LinearAllocator() {
        free(begin);
    }


    uint8_t* allocate(size_t blockSz) {
        if (getFreeSize() >= blockSz) {
            uint8_t* block = ptr;
            ptr += blockSz;
            return block;
        } else {
            ptr = end;
            throw NoMemoryException();
        }
    }

    template<typename T, typename... Args>
    inline T* create(Args... args) {
        uint8_t* block = allocate(sizeof(T));
        return new(block) T(args...);
    }

    void reset() {
        ptr = begin;
    }

    size_t getUsedSize() const {
        return ptr - begin;
    }

    // todo: test
    size_t getFreeSize() const {
        return end - ptr - 1;
    }

    const size_t getSize() const { return end - begin - 1; }

    const uint8_t* getBlock() const { return begin; }

private:
    uint8_t* begin;
    uint8_t* ptr;
    uint8_t* end;
};