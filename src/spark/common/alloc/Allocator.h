#pragma once
#include "MemBlock.h"
#include "NoMemoryException.h"

class Allocator {
public:
    virtual ~Allocator() = default;
    virtual MemBlock allocate(size_t sz) = 0;

    template<typename T, typename... Args>
    inline T* create(Args... args) {
        auto block = allocate(sizeof(T));
        return block.init<T>(args...);
    }
};
