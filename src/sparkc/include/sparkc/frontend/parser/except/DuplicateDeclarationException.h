#pragma once
#include "ParseException.h"
#include <sstream>

class DuplicateDeclarationException : public ParseException {
public:
    DuplicateDeclarationException(const Token& token)
        : ParseException(Kind::DuplicateDeclaration, token, buildMessage(token)) { }

private:
    static std::string buildMessage(const Token& token) {
        char nameBuf[32];
        token.value.copyTo(nameBuf, sizeof(nameBuf));

        std::ostringstream oss;
        oss << "Variable '" << nameBuf << "' already declared";
        return oss.str();
    }
};