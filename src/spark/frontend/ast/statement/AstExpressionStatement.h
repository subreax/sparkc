#pragma once
#include "AstStatement.h"
#include "../exp/AstExp.h"

class AstExpressionStatement : public AstStatement {
public:
    AstExpressionStatement(AstExp* exp) 
        : AstStatement(Type::Expression)
        , exp(exp) {}

    AstExp* getExpression() { return exp; }

private:
    AstExp* exp;
};