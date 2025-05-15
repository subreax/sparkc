#include "StringRef.h"

std::string operator+(const std::string& s1, StringRef s2) {
    int bufSz = std::min(32, s2.getLength() + 1);
    char* buf = (char*) alloca(bufSz);
    int len = s2.copyTo(buf, bufSz);
    return s1 + std::string(buf, len);
}

std::string operator+(StringRef s1, const std::string& s2) {
    return s2 + s1;
}
