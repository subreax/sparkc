#pragma once
#include "AstStatement.h"
#include "../exp/AstExp.h"

class AstReturnStatement : public AstStatement {
public:
    AstReturnStatement(AstExp* exp) 
        : AstStatement(Kind::Return)
        , exp(exp) { }

    AstExp* getExpression() { return exp; }
    SymbolType* getType() { return exp->type; }

private:
    AstExp* exp;
};