#pragma once
#include "alloc/Allocator.h"
#include "StringBuilder.h"
#include "StringRef.h"

class IdentifierGen {
public:
    IdentifierGen(Allocator& allocator) : allocator(allocator) { }

    StringRef unique(const char* id) {
        return unique(StringRef::cstr(id));
    }

    StringRef unique(StringRef id, const char* prefix) {
        char buf[MAX_ID_LEN];
        StringBuilder(buf, sizeof(buf))
            .append(StringRef::cstr(prefix))
            .append(StringRef::cstr("_"))
            .append(id);
        return unique(buf);
    }

    StringRef unique(StringRef id) {
        StringRef str = StringBuilder(nameBuf, sizeof(nameBuf))
            .append(id, std::min(id.getLength(), MAX_ID_LEN))
            .append(StringRef::cstr("."))
            .append(counter++)
            .toString();
        return copy(str);
    }

    StringRef copy(StringRef ref) {
        auto len = std::min(ref.getLength(), sizeof(nameBuf));
        return StringBuilder(allocator.allocate(len))
            .append(ref)
            .toString();
    }

    static constexpr size_t MAX_ID_LEN = 24;

private:
    char nameBuf[MAX_ID_LEN + 16];
    Allocator& allocator;
    int counter = 0;
};
