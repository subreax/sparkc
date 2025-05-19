#pragma once
#include <algorithm>
#include <string>

class StringRef {
public:
    StringRef(const char* str, size_t length) 
        : str(str), length(length) { }

    static StringRef nullInstance() {
        return StringRef(nullptr, 0);
    }

    size_t copyTo(char* out, size_t capacity) const {
        if (capacity == 0) {
            return 0;
        }

        if (isNotNull()) {
            size_t count = std::min(capacity - 1, length);
            size_t i;
            for (i = 0; i < count; i++) {
                out[i] = str[i];
            }
            out[i] = 0;
            return i;
        } else {
            out[0] = 0;
            return 0;
        }
    }

    bool isNotNull() const {
        return str != nullptr;
    }

    size_t getLength() const {
        return length;
    }

    const char* getReference() const {
        return str;
    }

    std::string toString(size_t maxLen = 32) const {
        char* buf = (char*) alloca(std::min(maxLen, length + 1));
        copyTo(buf, maxLen);
        return std::string(buf);
    }

private:
    const char* str;
    size_t length;
};


std::string operator+(const std::string& s1, StringRef s2);
std::string operator+(StringRef s1, const std::string& s2);

// capacity = 10
// len = 10
// count = min(9, 10) = 9


// capacity = 11
// len = 10
// count = min(10, 10) = 10


// capacity = 5
// len = 10
// count = min(4, 10) = 4