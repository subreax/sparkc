#pragma once
#include "AstExp.h"

class AstVar : public AstExp {
public:
    AstVar(const char* identifier) : AstExp(Type::EXP_VAR), identifier(identifier) {  }

    const char* getIdentifier() const { return identifier; }
    void setIdentifier(const char* id) { identifier = id; }

private:
    const char* identifier;
};