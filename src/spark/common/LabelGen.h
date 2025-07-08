#pragma once
#include "alloc/Allocator.h"
#include "StringBuilder.h"

class LabelGen {
public:
    LabelGen(Allocator& allocator) : allocator(allocator) {  }

    StringRef uniqueInternal(const char* id) {
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

    static bool isInternal(StringRef id) {
        return id.getReference() != nullptr && id.getReference()[0] == '.';
    }

    static bool isExternal(StringRef id) {
        return !isInternal(id);
    }

private:
    Allocator& allocator;
    int counter = 0;
    char buf[LABEL_MAX_LEN + 16];
};
