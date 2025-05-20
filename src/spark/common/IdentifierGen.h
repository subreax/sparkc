#pragma once
#include "LinearAllocator.h"
#include "CStringBuilder.h"

class IdentifierGen {
public:
    IdentifierGen(LinearAllocator& allocator) : allocator(allocator) { }

    const char* unique(const char* id) {
        auto len = CStringBuilder(nameBuf, sizeof(nameBuf))
            .append(id, MAX_ID_LEN)
            .append(".")
            .append(counter)
            .getLengthWith0();
        
        counter++;

        auto allocatedStr = (char*) allocator.allocate(len);
        CStringBuilder(allocatedStr, len).append(nameBuf);
        return allocatedStr;
    }

    static constexpr size_t MAX_ID_LEN = 16;

private:
    char nameBuf[MAX_ID_LEN + 16];
    LinearAllocator& allocator;
    int counter = 0;
};
