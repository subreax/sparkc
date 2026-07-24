#pragma once
#include "AstStatement.h"
#include "../exp/AstExp.h"

class AstWhileStatement : public AstStatement {
public:
    AstWhileStatement(AstExp* cond, AstStatement* statement)
        : AstStatement(AstStatement::Kind::While)
        , cond(cond)
        , statement(statement) { }

    AstExp* getCondition() { return cond; }
    void setCondition(AstExp* cond) { this->cond = cond; }

    AstStatement* getStatement() { return statement; }

private:
    AstExp* cond;
    AstStatement* statement;
};