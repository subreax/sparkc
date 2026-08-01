#pragma once
#include "StringRef.h"

class ExpandableStringBuilder {
public:
    ExpandableStringBuilder(size_t initialCap = 128)
        : _len(0)
        , _cap(initialCap) {
        increaseCapacity(initialCap);
    }

    ExpandableStringBuilder(const ExpandableStringBuilder& other)
        : _buf((char*) malloc(other._cap))
        , _len(other._len)
        , _cap(other._cap) {
        memcpy(_buf, other._buf, _len + 1);
    }

    ~ExpandableStringBuilder() {
        free(_buf);
    }

    ExpandableStringBuilder& append(const char* str) {
        size_t len = strnlen(str, 2048);
        return append(str, len);
    }

    ExpandableStringBuilder& append(StringRef ref) {
        return append(ref.getReference(), ref.getLength());
    }

    ExpandableStringBuilder& append(const std::string& str) {
        return append(str.c_str(), str.length());
    }

    ExpandableStringBuilder& append(int i) {
        char buf[16];
        snprintf(buf, sizeof(buf), "%d", i);
        return append(buf);
    }

    ExpandableStringBuilder& append(float f) {
        char buf[24];
        snprintf(buf, sizeof(buf), "%g", f);
        return append(buf);
    }

    size_t getLength() const {
        return _len;
    }

    const char* c_str() const {
        return _buf;
    }

    std::string toString() const {
        return std::string(_buf);
    }

private:
    ExpandableStringBuilder& append(const char* str, size_t len) {
        if (!fits(len)) {
            increaseCapacity((_len + len) * 2);
        }
        memcpy(_buf + _len, str, len);
        _len += len;
        _buf[_len] = '\0';
        return *this;
    }

    void increaseCapacity(size_t newCapacity) {
        _cap = newCapacity;
        _buf = (char*) realloc(_buf, newCapacity);
    }

    bool fits(size_t len) {
        return len + _len + 1 < _cap;
    }

    char* _buf = nullptr;
    size_t _len;
    size_t _cap;
};

inline ExpandableStringBuilder& operator<<(ExpandableStringBuilder& sb, const char* str) {
    return sb.append(str);
}

inline ExpandableStringBuilder& operator<<(ExpandableStringBuilder& sb, StringRef ref) {
    return sb.append(ref);
}

inline ExpandableStringBuilder& operator<<(ExpandableStringBuilder& sb, const std::string& str) {
    return sb.append(str);
}

inline ExpandableStringBuilder& operator<<(ExpandableStringBuilder& sb, int i) {
    return sb.append(i);
}

inline ExpandableStringBuilder& operator<<(ExpandableStringBuilder& sb, float f) {
    return sb.append(f);
}