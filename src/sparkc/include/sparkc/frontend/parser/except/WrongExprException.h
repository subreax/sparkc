#pragma once
#include "ParseException.h"

class WrongExprException : public ParseException {
public: 
    WrongExprException(const Token& token) 
        : ParseException(
            Kind::WrongExpression, 
            token, 
            "Failed to parse expression: wtf '" + token.value.toString() + "'"
        ) {}
};