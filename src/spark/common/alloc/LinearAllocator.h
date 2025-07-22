#pragma once
#include <memory>
#include <cstring>
#include "Allocator.h"
#include "../OutOfBoundsException.h"

class LinearAllocator : public Allocator {
public:
    LinearAllocator(const char* name, size_t sz, bool zeroMem = false)
        : name(name)
        , begin((uint8_t*) malloc(sz))
        , ptr(begin)
        , end(begin + sz)
    {
        if (zeroMem) {
            memset(begin, 0, sz);
        }
    }

    ~LinearAllocator() override {
        ::free(begin);
    }

    const char* getName() const { return name; }

    MemBlock allocate(size_t blockSz) {
        if (blockSz <= getFreeSize()) {
            uint8_t* block = ptr;
            ptr += blockSz;
            return MemBlock(blockSz, block);
        } else {
            throw NoMemoryException(name);
        }
    }

    void free(size_t sz) {
        if (sz <= getUsedSize()) {
            ptr -= sz;
        } else {
            throw OutOfBoundsException(0, getCapacity(), (int32_t) getUsedSize() - (int32_t) sz);
        }
    }

    void reset() override {
        ptr = begin;
    }

    size_t getUsedSize() const {
        return ptr - begin;
    }

    size_t getFreeSize() const {
        return end - ptr;
    }

    const size_t getCapacity() const { return end - begin; }

    uint8_t* getBlock() { return begin; }
    const uint8_t* getBlock() const { return begin; }

    uint8_t* getPtr() { return ptr; }
    const uint8_t* getPtr() const { return ptr; }

private:
    const char* name;
    uint8_t *const begin;
    uint8_t* ptr;
    uint8_t* end;
};