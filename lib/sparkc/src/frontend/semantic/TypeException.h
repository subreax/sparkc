#pragma once
#include "sparkc/common/SparkRuntimeException.h"

class TypeException : public SparkRuntimeException {
public:
    TypeException(const std::string& msg)
        : SparkRuntimeException(msg) { }
};
