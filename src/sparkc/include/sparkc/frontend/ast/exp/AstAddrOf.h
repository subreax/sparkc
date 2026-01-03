#pragma once
#include "AstExp.h"

class AstAddrOf : public AstExp {
public:
    AstAddrOf(AstExp* exp, SymbolType* type)
        : AstExp(Kind::AddrOf, type)
        , exp(exp) { }

    AstExp* getExpression() const { return exp; }

private:
    AstExp* exp;
};