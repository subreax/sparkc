#include "StringRef.h"

bool operator==(const StringRef& r1, const StringRef& r2) {
    if (r1.getLength() != r2.getLength()) {
        return false;
    }

    const char* s1 = r1.getReference();
    const char* s2 = r2.getReference();
    auto len = r1.getLength();
    for (size_t i = 0; i < len; i++) {
        if (s1[i] != s2[i]) {
            return false;
        }
    }
    return true;
}

bool operator!=(const StringRef& r1, const StringRef& r2) {
    return !(r1 == r2);
}

std::string operator+(const std::string& s1, StringRef s2) {
    size_t bufSz = std::min((size_t) 32, s2.getLength() + 1);
    char* buf = (char*) alloca(bufSz);
    int len = s2.copyTo(buf, bufSz);
    return s1 + std::string(buf, len);
}

std::string operator+(StringRef s1, const std::string& s2) {
    return s2 + s1;
}
