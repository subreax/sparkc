#pragma once
#include "AstStatement.h"
#include "../exp/AstExp.h"

class AstIfStatement : public AstStatement {
public:
    AstIfStatement(AstExp* condition, AstStatement* ifTrue, AstStatement* ifFalse = nullptr) 
        : AstStatement(Kind::If)
        , condition(condition)
        , ifTrue(ifTrue)
        , ifFalse(ifFalse) { }

    AstExp* getCondition() { return condition; }

    AstStatement* getTrueBranch() { return ifTrue; }

    // could be null
    AstStatement* getFalseBranch() { return ifFalse; }

private:
    AstExp* condition;
    AstStatement* ifTrue;
    AstStatement* ifFalse;
};