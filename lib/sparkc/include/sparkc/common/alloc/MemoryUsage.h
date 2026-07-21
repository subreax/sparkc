#pragma once

class MemoryUsage {
public:
    MemoryUsage(size_t used, size_t total)
        : used(used), total(total) {  }
    
    size_t getUsed() const { return used; }
    size_t getTotal() const { return total; }
    size_t getFree() const { return total - used; }

    int getUsageInPercents() const {
        return used * 100 / total;
    }

private:
    size_t used;
    size_t total;
};