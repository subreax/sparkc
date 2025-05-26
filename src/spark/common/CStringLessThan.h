#pragma once
#include <cstring>

class CStringLessThan {
public:
    bool operator()(const char* s1, const char* s2) const noexcept {
        return strncmp(s1, s2, 128) < 0;
    }
};