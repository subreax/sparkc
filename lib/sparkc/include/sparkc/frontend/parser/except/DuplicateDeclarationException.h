#pragma once
#include "ParseException.h"

class DuplicateDeclarationException : public ParseException {
public:
    DuplicateDeclarationException(const Token& token)
        : ParseException(
              Kind::DuplicateDeclaration,
              token,
              "Variable '" + token.value.toString() + "' already declared"
          ) { }
};
