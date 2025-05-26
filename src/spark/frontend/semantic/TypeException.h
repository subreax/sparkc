#pragma once
#include <stdexcept>

class TypeException : public std::runtime_error {
public:
    TypeException(const std::string& msg) 
        : std::runtime_error("")
        , msg(msg) {}

    const char* what() const noexcept override { return msg.c_str(); }

private:
    std::string msg;
};