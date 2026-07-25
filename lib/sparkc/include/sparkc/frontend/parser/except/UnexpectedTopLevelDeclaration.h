#pragma once
#include "ParseException.h"

class UnexpectedTopLevelDeclaration : public ParseException {
public:
    UnexpectedTopLevelDeclaration(const Token& token)
        : ParseException(
              Kind::UnexpectedTopLevelDeclaration,
              token,
              "Unexpected top-level declaration"
          ) { }
};
