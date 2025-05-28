#pragma once
#include "alloc/Allocator.h"
#include "CStringBuilder.h"

class LabelGen {
public:
    LabelGen(Allocator& allocator) : allocator(allocator) {  }

    const char* uniqueInternal(const char* id) {
        auto len = CStringBuilder(buf, sizeof(buf))
            .append(".")
            .append(id, LABEL_MAX_LEN)
            .append("_")
            .append(counter)
            .getLengthWith0();

        counter++;

        return CStringBuilder(allocator.allocate(len))
            .append(buf)
            .getString();
    }

    static constexpr int LABEL_MAX_LEN = 24;

    static bool isInternal(const char* id) {
        return id != nullptr && *id == '.';
    }

    static bool isExternal(const char* id) {
        return !isInternal(id);
    }

private:
    Allocator& allocator;
    int counter = 0;
    char buf[LABEL_MAX_LEN + 16];
};
