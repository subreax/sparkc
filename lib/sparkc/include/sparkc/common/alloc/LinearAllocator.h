#pragma once
#include "../OutOfBoundsException.h"
#include "Allocator.h"
#include <cstring>
#include <memory>

class LinearAllocator : public Allocator {
public:
    LinearAllocator(StringRef name, size_t sz, bool zeroMem = false)
        : Allocator(name)
        , begin((uint8_t*) malloc(sz))
        , ptr(begin)
        , end(begin + sz) {
        if (zeroMem) {
            memset(begin, 0, sz);
        }
    }

    LinearAllocator(const char* name, size_t sz, bool zeroMem = false)
        : LinearAllocator(StringRef::cstr(name), sz, zeroMem) { }

    ~LinearAllocator() override {
        ::free(begin);
    }

    MemBlockRef allocate(size_t blockSz) override {
        if (blockSz <= getFreeSize()) {
            uint8_t* block = ptr;
            ptr += blockSz;
            return MemBlockRef(blockSz, block);
        }
        else {
            throw NoMemoryException(getName());
        }
    }

    void free(size_t sz) {
        if (sz <= getUsedSize()) {
            ptr -= sz;
        }
        else {
            throw OutOfBoundsException(0, getCapacity(), (int32_t) getUsedSize() - (int32_t) sz);
        }
    }

    void reset() override {
        ptr = begin;
    }

    size_t getUsedSize() const override {
        return ptr - begin;
    }

    size_t getFreeSize() const override {
        return end - ptr;
    }

    size_t getCapacity() const override {
        return end - begin;
    }

    uint8_t* getBlock() { return begin; }
    const uint8_t* getBlock() const { return begin; }

    uint8_t* getPtr() { return ptr; }
    const uint8_t* getPtr() const { return ptr; }

private:
    uint8_t* const begin;
    uint8_t* ptr;
    uint8_t* end;
};