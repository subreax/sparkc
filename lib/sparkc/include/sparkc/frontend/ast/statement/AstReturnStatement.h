#pragma once
#include "../exp/AstExp.h"
#include "AstStatement.h"

class AstReturnStatement : public AstStatement {
public:
    AstReturnStatement(AstExp* exp)
        : AstStatement(Kind::Return)
        , exp(exp) { }

    AstExp* getExpression() { return exp; }
    void setExpression(AstExp* exp) { this->exp = exp; }

private:
    AstExp* exp;
};