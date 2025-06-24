#pragma once
#include "AstExp.h"

class AstDereference : public AstExp {
public:
    AstDereference(AstExp* exp, SymbolType* type) 
        : AstExp(Kind::Dereference, type)
        , exp(exp) {  }

    AstExp* getExpression() const { return exp; }

private:
    AstExp* exp;
};