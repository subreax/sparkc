#pragma once
#include <cstdint>
#include <string>
#include <cstdio>
#include "spark/common/alloc/LinearAllocator.h"

class MemUtils {
public:
    static void dump(const uint8_t* block, size_t sz, const std::string& outFile) {
        FILE* f;
        fopen_s(&f, outFile.c_str(), "wb");
        for (size_t i = 0; i < sz; i++) {
            fputc(block[i], f);
        }
        fclose(f);
    }

    static void dump(const LinearAllocator& allocator, const std::string& outFile) {
        dump(allocator.getBlock(), allocator.getFreeSize(), outFile);
    }
};
