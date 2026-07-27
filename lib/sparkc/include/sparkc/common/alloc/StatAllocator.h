#pragma once
#include "Allocator.h"
#include "MemoryStats.h"

template <typename T>
class StatAllocator : public Allocator {
public:
    template <typename... Args>
    StatAllocator(const char* name, Args... args)
        : Allocator(name)
        , allocator(name, args...) { }

    MemBlockRef allocate(size_t sz) override {
        return allocator.allocate(sz);
    }

    size_t getFreeSize() const override {
        return allocator.getFreeSize();
    }

    size_t getUsedSize() const override {
        return allocator.getUsedSize();
    }

    size_t getCapacity() const override {
        return allocator.getCapacity();
    }

    void reset() override {
        updatePeak();
        allocator.reset();
    }

    void resetPeak() {
        peak = 0;
    }

    MemoryStats getPeakMemoryStats() {
        updatePeak();
        return MemoryStats(peak, getCapacity());
    }

    T& getAllocator() { return allocator; }

private:
    void updatePeak() {
        peak = std::max(getUsedSize(), peak);
    }

    T allocator;
    size_t peak = 0;
};