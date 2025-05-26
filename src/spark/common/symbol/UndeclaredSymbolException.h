#pragma once
#include <stdexcept>

class UndeclaredSymbolException : public std::runtime_error {
public:
    UndeclaredSymbolException(const char* name)
        : std::runtime_error("")
        , message(std::string("Undeclared symbol: '") + name + "'") {  }

    const char* what() const noexcept override {
        return message.c_str();
    }

private:
    std::string message;
};
