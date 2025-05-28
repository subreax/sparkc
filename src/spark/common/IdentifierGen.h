#pragma once
#include "alloc/Allocator.h"
#include "CStringBuilder.h"
#include "StringRef.h"

class IdentifierGen {
public:
    IdentifierGen(Allocator& allocator) : allocator(allocator) { }

    const char* unique(const char* id) {
        return unique(id, strlen(id));
    }

    const char* unique(const char* id, const char* suffix) {
        char buf[MAX_ID_LEN + 2];
        CStringBuilder(buf, sizeof(buf))
            .append(id, MAX_ID_LEN - 4)
            .append("_")
            .append(suffix);

        return unique(buf);
    }

    const char* unique(StringRef ref) {
        return unique(ref.getReference(), ref.getLength());
    }

    const char* copy(StringRef ref) {
        auto len = std::min(ref.getLength() + 1, sizeof(nameBuf));
        return CStringBuilder(allocator.allocate(len))
            .append(ref)
            .getString();
    }

    static constexpr size_t MAX_ID_LEN = 24;

private:
    const char* unique(const char* id, size_t idLen) {
        size_t len = CStringBuilder(nameBuf, sizeof(nameBuf))
            .append(id, std::min(idLen, MAX_ID_LEN))
            .append(".")
            .append(counter)
            .getLengthWith0();
        
        counter++;

        return CStringBuilder(allocator.allocate(len))
            .append(nameBuf)
            .getString();
    }

    char nameBuf[MAX_ID_LEN + 16];
    Allocator& allocator;
    int counter = 0;
};
