#pragma once
#include "../StringRef.h"
#include "MemBlock.h"
#include "NoMemoryException.h"
#include "MemoryUsage.h"

class Allocator {
public:
    Allocator(const char* name)
        : name(StringRef::cstr(name)) { }

    Allocator(StringRef name)
        : name(name) { }

    virtual ~Allocator() = default;

    virtual MemBlock allocate(size_t sz) = 0;
    virtual void reset() = 0;

    virtual size_t getFreeSize() const = 0;
    virtual size_t getUsedSize() const = 0;
    virtual size_t getCapacity() const = 0;

    MemoryUsage getMemoryUsage() const {
        return MemoryUsage(getUsedSize(), getCapacity());
    }

    template <typename T, typename... Args>
    inline T* create(Args... args) {
        auto block = allocate(sizeof(T));
        return block.init<T>(args...);
    }

    StringRef getName() const { return name; }

private:
    StringRef name;
};
