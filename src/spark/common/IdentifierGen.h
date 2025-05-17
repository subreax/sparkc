#pragma once
#include "LinearAllocator.h"

class IdentifierGen {
public:
    IdentifierGen(LinearAllocator& allocator) : allocator(allocator) { }

    const char* unique(const char* id) {
        auto len = generate(id);
        auto allocatedStr = (char*) allocator.allocate(len + 1);
        if (allocatedStr == nullptr) {
            return ".err_out_of_mem";
        }

        strcpy(allocatedStr, nameBuf);
        return allocatedStr;
    }

    static constexpr size_t MAX_ID_LEN = 16;
    static constexpr const char* ERR_NO_MEM = ".err_out_of_mem";

private:
    size_t generate(const char* id) {
        size_t len = copy(nameBuf, id, MAX_ID_LEN);
        len += sprintf(nameBuf + len, ".%d", counter);
        counter++;
        return len;
    }

    static size_t copy(char* to, const char* from, size_t max) {
        size_t i = 0;
        for ( ; *from && i < max; i++) {
            to[i] = *from;
            from++;
        }
        return i;
    }

    char nameBuf[MAX_ID_LEN + 16];
    LinearAllocator& allocator;
    int counter = 0;
};
