#pragma once
#include "AstExp.h"

class AstCast : public AstExp {
public:
    AstCast(AstExp* exp, SymbolType* targetType)
        : AstExp(Kind::Cast, targetType)
        , exp(exp) { }

    AstExp* getExp() { return exp; }

private:
    AstExp* exp;
};