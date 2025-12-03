#pragma once
#include "../SparkRuntimeException.h"
#include "../StringRef.h"

class NoMemoryException : public SparkRuntimeException {
public:
    NoMemoryException(StringRef allocatorName) : SparkRuntimeException("Failed to allocate memory in allocator " + allocatorName.toString()) { }
};