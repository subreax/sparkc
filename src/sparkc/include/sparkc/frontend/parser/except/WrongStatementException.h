#pragma once
#include "ParseException.h"

class WrongStatementException : public ParseException {
public:
    WrongStatementException(const Token& token)
        : ParseException(
            Kind::WrongStatement,
            token, 
            "Failed to parse statement: wtf '" + token.value.toString() + "'"
        ) {}
};
