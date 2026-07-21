#pragma once
#include "Allocator.h"
#include "MemoryUsage.h"

template <typename T>
class StatAllocator : public Allocator {
public:
    template <typename... Args>
    StatAllocator(const char* name, Args... args)
        : Allocator(name)
        , allocator(name, args...) { }

    MemBlock allocate(size_t sz) override {
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
        peak = calcPeak();
        allocator.reset();
    }

    void resetPeak() {
        peak = 0;
    }

    MemoryUsage getPeakMemoryUsage() const {
        return MemoryUsage(peak, getCapacity());
    }

    T& getAllocator() { return allocator; }

private:
    size_t calcPeak() const {
        return std::max(getUsedSize(), peak);
    }

    T allocator;
    size_t peak = 0;
};