#pragma once
#include "../exp/AstExp.h"
#include "AstStatement.h"

class AstWhileStatement : public AstStatement {
public:
    AstWhileStatement(AstExp* cond, AstStatement* statement)
        : AstStatement(AstStatement::Kind::While)
        , cond(cond)
        , statement(statement) { }

    AstExp* getCondition() { return cond; }
    AstStatement* getStatement() { return statement; }

private:
    AstExp* cond;
    AstStatement* statement;
};