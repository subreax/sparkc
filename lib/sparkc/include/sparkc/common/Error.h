#pragma once
#include <string>
#include <cstdarg>
#include <cstdio>

void sparkError(const char* fileName, const char* fmt, ...);

inline void sparkError(const char* fileName, const std::string& str) {
    sparkError(fileName, str.c_str());
}
