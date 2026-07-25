#pragma once
#include "ParseException.h"
#include <sstream>

class UndeclaredVariableException : public ParseException {
public:
    UndeclaredVariableException(const Token& token)
        : ParseException(
              Kind::UndeclaredVariable,
              token,
              "Undeclared variable: '" + token.value.toString() + "'"
          ) { }
};
