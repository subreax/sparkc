#pragma once
#include <cstdint>
#include <stdexcept>
#include <vector>
#include "alloc/Allocator.h"

template<typename BoundArray>
class BaConstIterator {
public:
    using ItemType = typename BoundArray::ItemType;

    BaConstIterator(ItemType* ptr) : ptr(ptr) {  }

    bool operator==(BaConstIterator it1) const {
        return ptr == it1.ptr;
    }

    bool operator!=(BaConstIterator it1) const {
        return !(*this == it1);
    }

    const BaConstIterator& operator++() {
        ptr += 1;
        return *this;
    }

    const ItemType& operator*() const {
        return *ptr;
    }

private:
    ItemType* ptr;
};

template<typename BoundArray>
class BaIterator : public BaConstIterator<BoundArray> {
public:
    using ItemType = typename BoundArray::ItemType;
    using BaConstIterator<BoundArray>::BaConstIterator;

    BaIterator& operator++() {
        BaConstIterator<BoundArray>::operator++();
        return *this;
    }

    ItemType& operator*() {
        return const_cast<ItemType&>(BaConstIterator<BoundArray>::operator*());
    }
};


template<typename T>
class BoundArray {
public:
    using ItemType = T;
    using Iterator = BaIterator<BoundArray<T>>;
    using ConstIterator = BaConstIterator<BoundArray<T>>;

    BoundArray(MemBlock block) : mem((T*) block.mem), itemsCount(block.sz / sizeof(T)) {  }

    static BoundArray<T> create(size_t itemsCount, Allocator& allocator) {
        return BoundArray<T>(allocator.allocate(itemsCount * sizeof(T)));
    }

    static BoundArray<T> fromVector(const std::vector<T>& v, Allocator& allocator) {
        auto ba = create(v.size(), allocator);
        for (size_t i = 0; i < v.size(); i++) {
            ba[i] = v[i];
        }
        return ba;
    }

    std::vector<T> toVector() const {
        std::vector<T> v(itemsCount);
        for (size_t i = 0; i < itemsCount; i++) {
            v[i] = (*this)[i];
        }
        return v;
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

    void set(size_t idx, const T& value) {
        if (idx < itemsCount) {
            mem[idx] = value;
        }
        throw std::out_of_range("BoundArray index is out of range");
    }

    size_t size() const { return itemsCount; }

    Iterator begin() {
        return Iterator(mem);
    }

    ConstIterator begin() const {
        return ConstIterator(mem);
    }

    Iterator end() {
        return Iterator(mem + itemsCount);
    }

    ConstIterator end() const {
        return ConstIterator(mem + itemsCount);
    }

private:
    T* mem;
    size_t itemsCount;
};