#pragma once
#include "../exp/AstExp.h"
#include "AstStatement.h"

class AstExpressionStatement : public AstStatement {
public:
    AstExpressionStatement(AstExp* exp)
        : AstStatement(Kind::Expression)
        , exp(exp) { }

    AstExp* getExpression() { return exp; }

private:
    AstExp* exp;
};