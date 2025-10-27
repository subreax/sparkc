#pragma once
#include <string>
#include "../../spark/common/LabelGen.h"

class Colored {
public:
    static std::string label(StringRef s) {
        if (LabelGen::isPublic(s)) {
            return "\033[32m" + s.toString() + "\033[0m";
        } else {
            return "\033[31m" + s.toString() + "\033[0m";
        }
    }

    static std::string comment(const std::string& s) {
        return "\033[2m" + s + "\033[0m";
    }
};