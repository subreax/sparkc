#pragma once
#include <string>
#include "../spark/common/LabelGen.h"

class Colored {
public:
    static std::string label(const char* label) {
        if (LabelGen::isInternal(label)) {
            return std::string("\033[32m") + label + "\033[0m";
        } else {
            return std::string("\033[31m") + label + "\033[0m";
        }
    }
};