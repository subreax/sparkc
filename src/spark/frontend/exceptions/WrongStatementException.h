#pragma once
#include "ParserException.h"

class WrongStatementException : public ParserException {
public:
    WrongStatementException(const Token& token)
        : ParserException(
            Type::WrongStatement,
            token, 
            "Failed to parse statement: wtf '" + token.value.toString() + "'"
        ) {}
};
