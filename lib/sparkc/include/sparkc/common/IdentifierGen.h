#pragma once
#include "StringBuilder.h"
#include "StringRef.h"
#include "alloc/Allocator.h"

class IdentifierGen {
public:
    IdentifierGen(Allocator& allocator)
        : allocator(allocator) { }

    StringRef unique(const char* id) {
        return unique(StringRef::cstr(id));
    }

    StringRef unique(StringRef id, const char* prefix) {
        char buf[MAX_ID_LEN];
        return unique(
            StringBuilder(buf, sizeof(buf))
                .append(StringRef::cstr(prefix))
                .append(StringRef::cstr("_"))
                .append(id)
                .toString()
        );
    }

    StringRef unique(StringRef id) {
        return copy(
            StringBuilder(nameBuf, sizeof(nameBuf))
                .append(id, std::min(id.getLength(), MAX_ID_LEN))
                .append(StringRef::cstr("."))
                .append(counter++)
                .toString()
        );
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
