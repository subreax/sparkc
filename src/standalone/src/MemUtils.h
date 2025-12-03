#pragma once
#include <cstdint>
#include <string>
#include <cstdio>
#include <sparkc/common/alloc/LinearAllocator.h>

class MemUtils {
public:
    static void dump(const uint8_t* block, size_t sz, const std::string& outFile) {
        FILE* f;
        fopen_s(&f, outFile.c_str(), "wb");
        fwrite(block, 1, sz, f);
        fclose(f);
    }

    static void dump(const LinearAllocator& allocator, const std::string& outFile) {
        dump(allocator.getBlock(), allocator.getFreeSize(), outFile);
    }
};
