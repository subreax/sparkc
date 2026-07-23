#pragma once
#include "../exp/AstExp.h"
#include "AstStatement.h"

class AstExpressionStatement : public AstStatement {
public:
    AstExpressionStatement(AstExp* exp)
        : AstStatement(Kind::Expression)
        , exp(exp) { }

    AstExp* getExpression() { return exp; }
    void setExpression(AstExp* exp) { this->exp = exp; }

private:
    AstExp* exp;
};