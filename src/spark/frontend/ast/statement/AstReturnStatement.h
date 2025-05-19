#pragma once
#include "AstStatement.h"
#include "../exp/AstExp.h"

class AstReturnStatement : public AstStatement {
public:
    AstReturnStatement(AstExp* exp) 
        : AstStatement(Type::Return)
        , exp(exp) { }

    AstExp* getExpression() { return exp; }

private:
    AstExp* exp;
};