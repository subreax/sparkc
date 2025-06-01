#pragma once
#include <vector>
#include "AstBlockItem.h"
#include "AstFunParam.h"
#include "AstBlock.h"
#include "../../common/BoundArray.h"

class AstFunction {
public:
    AstFunction(const char* name, SymbolType* retType, BoundArray<AstFunParam*> params, AstBlock* block)
        : name(name)
        , retType(retType)
        , params(params)
        , block(block) {  }

    const char* getName() const { return name; }
    SymbolType* getReturnType() const { return retType; }
    const BoundArray<AstFunParam*>& getParams() const { return params; }
    const AstBlock* getBlock() const { return block; }

private:
    const char* name;
    SymbolType* retType;
    BoundArray<AstFunParam*> params;
    AstBlock* block;
};
