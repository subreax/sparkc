#pragma once
#include "SparkRuntimeException.h"
#include <sstream>

class OutOfBoundsException : public SparkRuntimeException {
public:
    OutOfBoundsException(int32_t min, int32_t max, int32_t actual)
        : SparkRuntimeException(buildMessage(min, max, actual)) { }

private:
    static std::string buildMessage(int32_t min, int32_t max, int32_t actual) {
        std::ostringstream oss;
        oss << "Index is out of bounds. Range: [" << min << "; " << max << "], actual: " << actual;
        return oss.str();
    }
};
