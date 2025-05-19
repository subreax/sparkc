#pragma once
#include "ParserException.h"

class WrongExprException : public ParserException {
public: 
    WrongExprException(const Token& token) 
        : ParserException(
            Type::WrongExpression, 
            token, 
            "Failed to parse expression: wtf '" + token.value.toString() + "'"
        ) {}
};