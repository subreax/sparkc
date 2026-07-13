#pragma once
#include "StringBuilder.h"
#include "alloc/Allocator.h"

class LabelGen {
public:
    LabelGen(Allocator& allocator)
        : allocator(allocator) { }

    StringRef uniquePrivate(const char* id) {
        StringRef str = StringBuilder(buf, sizeof(buf))
                            .append(StringRef::cstr("."))
                            .append(StringRef::cstr(id), LABEL_MAX_LEN)
                            .append(StringRef::cstr("_"))
                            .append(counter++)
                            .toString();

        return StringBuilder(allocator.allocate(str.getLength()))
            .append(str)
            .toString();
    }

    static constexpr int LABEL_MAX_LEN = 24;

    static bool isPrivate(StringRef id) {
        return id.getReference() != nullptr && id.getReference()[0] == '.';
    }

    static bool isPublic(StringRef id) {
        return !isPrivate(id);
    }

private:
    Allocator& allocator;
    int counter = 0;
    char buf[LABEL_MAX_LEN + 16];
};
