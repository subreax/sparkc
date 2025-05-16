#pragma once
#include "LinearAllocator.h"

class IdentifierGen {
public:
    IdentifierGen(LinearAllocator& allocator) : allocator(allocator) { }

    const char* unique(const char* id) {
        int nameLen = strlen(id);
        if (nameLen >= MAX_ID_LEN) {
            return ".err_name_too_long";
        }

        int len = sprintf(nameBuf, "%s.%d", id, counter);
        if (len < 0) {
            return ".err_failed_to_build";
        }

        counter++;

        auto allocatedStr = (char*) allocator.allocate(len + 1);
        if (allocatedStr == nullptr) {
            return ".err_out_of_mem";
        }

        strcpy(allocatedStr, nameBuf);
        return allocatedStr;
    }

    static constexpr int MAX_ID_LEN = 28;

private:
    char nameBuf[40];
    LinearAllocator& allocator;
    int counter = 0;
};
