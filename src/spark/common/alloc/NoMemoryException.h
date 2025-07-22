#pragma once
#include "../SparkRuntimeException.h"

class NoMemoryException : public SparkRuntimeException {
public:
    NoMemoryException(const char* allocatorName) : SparkRuntimeException("Failed to allocate memory in allocator " + std::string(allocatorName)) { }
};