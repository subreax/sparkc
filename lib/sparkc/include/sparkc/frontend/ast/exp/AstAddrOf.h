#pragma once
#include "AstExp.h"

class AstAddrOf : public AstExp {
public:
    AstAddrOf(AstExp* exp, SymbolType* type)
        : AstExp(Kind::AddrOf, type)
        , exp(exp) { }

    AstExp* getExp() const { return exp; }
    void setExp(AstExp* exp) { this->exp = exp; }

private:
    AstExp* exp;
};