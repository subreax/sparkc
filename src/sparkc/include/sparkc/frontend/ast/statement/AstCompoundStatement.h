#pragma once
#include "AstStatement.h"
#include "../AstBlock.h"
#include "sparkc/common/BoundArray.h"

class AstCompoundStatement : public AstStatement {
public:
    AstCompoundStatement(AstBlock* block) 
        : AstStatement(Kind::Compound)
        , block(block) {  }

    AstBlock* getBlock() { return block; }

private:
    AstBlock* block;
};