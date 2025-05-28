#pragma once
#include "../SparkRuntimeException.h"

class NoMemoryException : public SparkRuntimeException {
public:
    NoMemoryException() : SparkRuntimeException("Failed to allocate memory") { }
    NoMemoryException(const char* msg) : SparkRuntimeException(msg) { }
    NoMemoryException(const std::string& msg) : SparkRuntimeException(msg) { }
};