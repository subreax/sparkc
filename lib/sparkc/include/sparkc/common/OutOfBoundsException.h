#pragma once
#include "SparkRuntimeException.h"
#include <cstdint>
#include "sparkc/common/ExpandableStringBuilder.h"

class OutOfBoundsException : public SparkRuntimeException {
public:
    OutOfBoundsException(int32_t min, int32_t max, int32_t actual)
        : SparkRuntimeException(buildMessage(min, max, actual)) { }

private:
    static std::string buildMessage(int32_t min, int32_t max, int32_t actual) {
        ExpandableStringBuilder sb;
        sb << "Index is out of bounds. Range: [" << min << "; " << max << "], actual: " << actual;
        return sb.toString();
    }
};
