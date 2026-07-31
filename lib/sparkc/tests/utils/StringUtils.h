#pragma once
#include <string>

class StringUtils {
public:
    static std::string trim(const std::string& str) {
        size_t b = 0, e = str.length();
        while (b < e && std::isspace(str[b])) {
            b++;
        }

        while (e > b && std::isspace(str[e - 1])) {
            e--;
        }

        return str.substr(b, e - b);
    }
};