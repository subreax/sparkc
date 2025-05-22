#pragma once
#include <string>

class Colored {
public:
    static std::string label(const char* label) {
        return std::string("\033[32m") + label + "\033[0m";
    }
};