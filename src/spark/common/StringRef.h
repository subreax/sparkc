#pragma once
#include <algorithm>
#include <string>

class StringRef {
public:
    StringRef(const char* str, int length) 
        : str(str), length(length) { }

    static StringRef nullInstance() {
        return StringRef(nullptr, 0);
    }

    int copyTo(char* out, int capacity) const {
        if (capacity == 0) {
            return 0;
        }

        if (isNotNull()) {
            int count = std::min(capacity - 1, length);
            int i;
            for (i = 0; i < count; i++) {
                out[i] = str[i];
            }
            out[i] = 0;
            return i;
        } else {
            out[0] = 0;
            return 0;
        }
    }

    bool isNotNull() const {
        return str != nullptr;
    }

    int getLength() const {
        return length;
    }

    const char* getReference() const {
        return str;
    }

private:
    const char* str;
    int length;
};


std::string operator+(const std::string& s1, StringRef s2);
std::string operator+(StringRef s1, const std::string& s2);

// capacity = 10
// len = 10
// count = min(9, 10) = 9


// capacity = 11
// len = 10
// count = min(10, 10) = 10


// capacity = 5
// len = 10
// count = min(4, 10) = 4