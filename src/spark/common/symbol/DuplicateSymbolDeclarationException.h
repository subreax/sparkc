#pragma once
#include <stdexcept>
#include "SymbolType.h"

class DuplicateSymbolDeclarationException : public std::runtime_error {
public:
    DuplicateSymbolDeclarationException(const char* id, SymbolType* type)
        : std::runtime_error("")
        , message(buildMessage(id, type)) {  }

    const char* what() const noexcept override {
        return message.c_str();
    }

    const char* getIdentifier() const { return id; }
    const SymbolType* getType() const { return type; }

private:
    std::string buildMessage(const char* id, SymbolType* type) {
        return std::string("Symbol already declared: ") + id;
    }

    std::string message;
    const char* id;
    SymbolType* type;
};
