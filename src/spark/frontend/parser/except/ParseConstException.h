#pragma once
#include "ParseException.h"

class ParseConstException : public ParseException {
public:
    ParseConstException(const Token& token) 
        : ParseException(Kind::WrongConst, token, "Failed to parse constant: '" + token.value.toString() + "'") {  }
};