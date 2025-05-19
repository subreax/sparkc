#pragma once
#include <stdexcept>

class NoMemoryException : public std::runtime_error {
public:
    NoMemoryException() : std::runtime_error("Failed to allocate memory") { }
};