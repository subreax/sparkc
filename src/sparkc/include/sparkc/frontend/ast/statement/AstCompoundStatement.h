#pragma once
#include "../AstBlock.h"
#include "AstStatement.h"
#include "sparkc/common/BoundArray.h"

class AstCompoundStatement : public AstStatement {
public:
    AstCompoundStatement(AstBlock* block)
        : AstStatement(Kind::Compound)
        , block(block) { }

    AstBlock* getBlock() { return block; }

private:
    AstBlock* block;
};