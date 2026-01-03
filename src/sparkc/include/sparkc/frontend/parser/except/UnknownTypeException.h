#pragma once
#include "ParseException.h"

class UnknownTypeException : public ParseException {
public:
    UnknownTypeException(const Token& token)
        : ParseException(Kind::UnknownType, token, "Unknown type") { }
};