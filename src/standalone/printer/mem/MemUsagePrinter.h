#pragma once
#include <iostream>
#include "../../../spark/common/alloc/StatAllocator.h"

class MemUsagePrinter {
public:
    static void print(const std::string& name, size_t used, size_t cap) {
        auto percentage = used * 100 / cap;
        printf("%-13s", name.c_str());
        
        char buf[16];
        snprintf(buf, sizeof(buf), "%5d ", used);
        printf("%5s ", buf);

        const int w = cap * MEM_PROGRESS_BAR_4KB_SPACES / 4096;
        std::cout << "[";
        for (int i = 0; i < w; i++) {
            if (i * 100 / (w - 1) < percentage) {
                std::cout << "#";
            }
            else {
                std::cout << ".";
            }
        }
        std::cout << "] " << std::endl;
    }
    
    template<typename T>
    static void print(const StatAllocator<T>& allocator) {
        print(allocator.getName().toString(), allocator.getUsedSize(), allocator.getCapacity());
    }

private:
    static constexpr int MEM_PROGRESS_BAR_4KB_SPACES = 45;
};
