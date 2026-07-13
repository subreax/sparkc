#pragma once
#include <stdexcept>

class SparkRuntimeException : public std::runtime_error {
public:
    SparkRuntimeException(const std::string& msg)
        : std::runtime_error("")
        , msg(msg) { }

    SparkRuntimeException(const char* msg)
        : std::runtime_error("")
        , msg(msg) { }

    const char* what() const noexcept override { return msg.c_str(); }

private:
    std::string msg;
};
