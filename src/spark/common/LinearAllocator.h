#pragma once
#include <memory>

class LinearAllocator {
public:
    LinearAllocator(size_t sz) {
        begin = (uint8_t*) malloc(sz);
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
            return nullptr;
        }
    }

    template<typename T, typename... Args>
    inline T* create(Args... args) {
        uint8_t* block = allocate(sizeof(T));
        return new(block) T(args...);
    }

    void freeAll() {
        ptr = begin;
    }

    // todo: test
    size_t getFreeSize() const {
        return end - ptr;
    }

private:
    uint8_t* begin;
    uint8_t* ptr;
    uint8_t* end;
};