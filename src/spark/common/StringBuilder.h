#pragma once
#include <cstring>
#include <cstdint>
#include <cstdlib>
#include "StringRef.h"
#include "alloc/MemBlock.h"

class StringBuilder {
public:
    StringBuilder(char* buf, size_t capacity) 
        : buf(buf)
        , capacity(capacity) {  }

    StringBuilder(MemBlock block) 
        : StringBuilder((char*) block.mem, block.sz) {  }

    StringBuilder& append(StringRef str, size_t maxChars = SIZE_MAX) {
        return append(str.getReference(), str.getLength(), maxChars);
    }

    StringBuilder& append(int32_t i) {
        char strInt[16];
        itoa(i, strInt, 10);
        return append(StringRef::cstr(strInt, sizeof(strInt)));
    }

    size_t getLength() const { return len; }

    StringRef toString() { return StringRef(buf, len); }

private:
    StringBuilder& append(const char* str, size_t strLength, size_t maxChars) {
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
    size_t capacity;
};
