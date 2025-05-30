#pragma once
#include "AstExp.h"

class AstVar : public AstExp {
public:
    AstVar(const char* identifier) : AstExp(Kind::Var), identifier(identifier) {  }

    const char* getIdentifier() const { return identifier; }

private:
    const char* identifier;
};