#pragma once
#include <sparkc/common/StringRef.h>

class StringBuilder {
public:
    StringBuilder(size_t initialCap = 128)
        : _len(0)
        , _cap(initialCap) {
        increaseCapacity(initialCap);
    }

    StringBuilder(const StringBuilder& other)
        : _pool((char*) malloc(other._cap))
        , _len(other._len)
        , _cap(other._cap) {
        memcpy(_pool, other._pool, _len + 1);
    }

    ~StringBuilder() {
        free(_pool);
    }

    StringBuilder& append(const char* str) {
        size_t len = strnlen(str, 2048);
        return append(str, len);
    }

    StringBuilder& append(StringRef ref) {
        return append(ref.getReference(), ref.getLength());
    }

    StringBuilder& append(const std::string& str) {
        return append(str.c_str(), str.length());
    }

    StringBuilder& append(int i) {
        char buf[16];
        snprintf(buf, sizeof(buf), "%d", i);
        return append(buf);
    }

    StringBuilder& append(float f) {
        char buf[24];
        snprintf(buf, sizeof(buf), "%g", f);
        return append(buf);
    }

    size_t getLength() const {
        return _len;
    }

    const char* c_str() const {
        return _pool;
    }

    std::string toString() const {
        return std::string(_pool);
    }

private:
    StringBuilder& append(const char* str, size_t len) {
        if (!fits(len)) {
            increaseCapacity((_len + len) * 2);
        }
        memcpy(_pool + _len, str, len);
        _len += len;
        _pool[_len] = '\0';
        return *this;
    }

    void increaseCapacity(size_t newCapacity) {
        _cap = newCapacity;

        _pool = (char*) realloc(_pool, newCapacity);
    }

    bool fits(size_t len) {
        return len + _len + 1 < _cap;
    }

    char* _pool = nullptr;
    size_t _len;
    size_t _cap;
};

inline StringBuilder& operator<<(StringBuilder& sb, const char* str) {
    return sb.append(str);
}

inline StringBuilder& operator<<(StringBuilder& sb, StringRef ref) {
    return sb.append(ref);
}

inline StringBuilder& operator<<(StringBuilder& sb, const std::string& str) {
    return sb.append(str);
}

inline StringBuilder& operator<<(StringBuilder& sb, int i) {
    return sb.append(i);
}

inline StringBuilder& operator<<(StringBuilder& sb, float f) {
    return sb.append(f);
}