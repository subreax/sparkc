#pragma once
#include "alloc/LinearAllocator.h"
#include <stdexcept>

template <typename T>
class Stack {
public:
    Stack(LinearAllocator& allocator)
        : allocator(allocator) { }

    ~Stack() {
        allocator.free(sz * sizeof(T));
    }

    size_t getSize() const { return sz; }

    void push(const T& value) {
        T* ptr = (T*) allocator.allocate(sizeof(T)).mem;
        *ptr = value;
        sz++;
    }

    void pop() {
        if (sz > 0) {
            allocator.free(sizeof(T));
            sz--;
        }
        else {
            throw std::out_of_range("Pop failed: out of range");
        }
    }

    T& peek(size_t offset = 0) {
        if (offset < sz) {
            return *(getTopElement() - offset);
        }
        throw std::out_of_range("Peek failed: out of range");
    }

    const T& peek(size_t offset = 0) const {
        if (offset < sz) {
            return *(getTopElement() - offset);
        }
        throw std::out_of_range("Peek failed: out of range");
    }

    bool isNotEmpty() const {
        return sz > 0;
    }

private:
    inline T* getTopElement() { return ((T*) allocator.getPtr()) - 1; }
    inline const T* getTopElement() const { return ((T*) allocator.getPtr()) - 1; }

    LinearAllocator& allocator;
    size_t sz = 0;
};
