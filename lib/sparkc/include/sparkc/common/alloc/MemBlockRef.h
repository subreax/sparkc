#pragma once
#include <cstdint>
#include <stdexcept>

struct MemBlockRef {
    MemBlockRef(size_t sz, uint8_t* mem)
        : sz(sz)
        , mem(mem) { }

    template <typename T, typename... Args>
    inline T* init(Args... args) {
        if (sz <= sizeof(T)) {
            return new (mem) T(args...);
        }
        throw std::out_of_range("Can't init object in MemBlockRef");
    }

    const size_t sz;
    uint8_t* const mem;
};