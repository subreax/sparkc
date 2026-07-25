#pragma once
#include "ParseException.h"
#include <sstream>

class DuplicateDeclarationException : public ParseException {
public:
    DuplicateDeclarationException(const Token& token)
        : ParseException(
              Kind::DuplicateDeclaration,
              token,
              "Variable '" + token.value.toString() + "' already declared"
          ) { }
};
