#pragma once
#include <cstring>
#include <cstdint>
#include <cstdlib>

class CStringBuilder {
public:
    CStringBuilder(char* buf, size_t capacity) 
        : buf(buf)
        , capacity(capacity - 1) // null-terminator
        {  }

    CStringBuilder& append(const char* str, size_t maxChars = SIZE_MAX) {
        size_t i = 0;
        while (len < capacity && *str && i < maxChars) {
            buf[len] = *str;
            ++len;
            ++str;
            ++i;
        }
        buf[len] = 0;
        return *this;
    }

    CStringBuilder& append(int32_t i) {
        char strInt[16];
        itoa(i, strInt, 10);
        return append(strInt);
    }

    size_t getLengthWithout0() const { return len; }
    size_t getLengthWith0() const { return len + 1; }

private:
    char* buf;
    size_t len = 0;
    size_t capacity;
};
