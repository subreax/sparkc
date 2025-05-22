#pragma once
#include <sstream>
#include "ParserException.h"

class DuplicateDeclarationException : public ParserException {
public:
    DuplicateDeclarationException(const Token& token) 
        : ParserException(Type::DuplicateDeclaration, token, buildMessage(token)) {  }

private:
    static std::string buildMessage(const Token& token) {
        char nameBuf[32];
        token.value.copyTo(nameBuf, sizeof(nameBuf));
        
        std::ostringstream oss;
        oss << "Variable '" << nameBuf << "' already declared";
        return oss.str();
    }
};