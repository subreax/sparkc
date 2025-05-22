#pragma once
#include "LinearAllocator.h"
#include "CStringBuilder.h"
#include "StringRef.h"

class IdentifierGen {
public:
    IdentifierGen(LinearAllocator& allocator) : allocator(allocator) { }

    const char* unique(const char* id) {
        return unique(id, strlen(id));
    }

    const char* unique(const char* id, size_t idLen) {
        size_t len = CStringBuilder(nameBuf, sizeof(nameBuf))
            .append(id, std::min(idLen, MAX_ID_LEN))
            .append(".")
            .append(counter)
            .getLengthWith0();
        
        counter++;

        auto allocatedStr = (char*) allocator.allocate(len);
        CStringBuilder(allocatedStr, len).append(nameBuf);
        return allocatedStr;
    }

    const char* unique(StringRef ref) {
        return unique(ref.getReference(), ref.getLength());
    }

    static constexpr size_t MAX_ID_LEN = 16;

private:
    char nameBuf[MAX_ID_LEN + 16];
    LinearAllocator& allocator;
    int counter = 0;
};
