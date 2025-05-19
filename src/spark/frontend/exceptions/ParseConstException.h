#pragma once
#include "ParserException.h"
#include "../Token.h"

class ParseConstException : public ParserException {
public:
    ParseConstException(const Token& token) 
        : ParserException(Type::WrongConst, token, "Failed to parse constant: '" + token.value.toString() + "'") {  }
};