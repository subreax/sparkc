#pragma once
#include <algorithm>
#include <string>
#include <cstring>

class StringRef {
public:
    StringRef(const char* str, size_t length) 
        : str(str), length(length) { }

    static StringRef nullInstance() {
        return StringRef(nullptr, 0);
    }

    static StringRef cstr(const char* str, size_t maxLen = 1024) {
        return StringRef(str, lengthOf(str, maxLen));
    }

    static size_t lengthOf(const char* str, size_t maxLen = 1024) {
        return strnlen(str, maxLen);
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

    bool operator==(const StringRef& other) const {
        if (getLength() != other.getLength()) {
            return false;
        }

        const char* s2 = other.getReference();
        for (size_t i = 0; i < length; i++) {
            if (str[i] != s2[i]) {
                return false;
            }
        }
        return true;
    }

    bool operator!=(const StringRef& other) const {
        return !(*this == other);
    }

private:
    const char* str;
    size_t length;
};

std::string operator+(const std::string& s1, StringRef s2);
std::string operator+(StringRef s1, const std::string& s2);


template <>
struct std::hash<StringRef> {
    // FNV hash
    // http://www.isthe.com/chongo/tech/comp/fnv/
    std::size_t operator()(const StringRef& k) const {
        std::size_t hash = 2166136261u;
        const unsigned char* str = (const unsigned char*) k.getReference();
        for (size_t i = 0; i < k.getLength(); i++) {
            hash = (hash ^ str[i]) * 16777619;
        }
        return hash;
    }
};
