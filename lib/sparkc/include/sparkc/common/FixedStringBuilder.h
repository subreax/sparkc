#pragma once
#include "StringRef.h"
#include "alloc/MemBlockRef.h"

class FixedStringBuilder {
public:
    FixedStringBuilder(char* buf, size_t capacity)
        : buf(buf)
        , capacity(capacity) { }

    FixedStringBuilder(MemBlockRef block)
        : FixedStringBuilder((char*) block.mem, block.sz) { }

    FixedStringBuilder& append(StringRef str, size_t maxChars = SIZE_MAX) {
        return append(str.getReference(), str.getLength(), maxChars);
    }

    FixedStringBuilder& append(int32_t i) {
        char strInt[16];
        itoa(i, strInt, 10);
        return append(StringRef::cstr(strInt, sizeof(strInt)));
    }

    size_t getLength() const { return len; }

    StringRef toString() { return StringRef(buf, len); }

private:
    FixedStringBuilder& append(const char* str, size_t strLength, size_t maxChars) {
        size_t lim = std::min(strLength, maxChars);
        size_t i = 0;
        while (len < capacity && *str && i < lim) {
            buf[len] = *str;
            ++len;
            ++str;
            ++i;
        }
        return *this;
    }

    char* buf;
    size_t len = 0;
    const size_t capacity;
};
