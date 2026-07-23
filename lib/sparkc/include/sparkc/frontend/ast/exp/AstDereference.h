#pragma once
#include "AstExp.h"

class AstDereference : public AstExp {
public:
    AstDereference(AstExp* exp, SymbolType* type)
        : AstExp(Kind::Dereference, type)
        , exp(exp) { }

    AstExp* getExp() const { return exp; }
    void setExp(AstExp* exp) { this->exp = exp; }

private:
    AstExp* exp;
};