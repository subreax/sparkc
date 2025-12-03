#pragma once
#include "AstProgItem.h"
#include <vector>
#include "AstBlockItem.h"
#include "AstFunParam.h"
#include "AstBlock.h"
#include "sparkc/common/BoundArray.h"

class AstFunction : public AstProgItem {
public:
    AstFunction(StringRef name, SymbolType* retType, BoundArray<AstFunParam*> params, AstBlock* block)
        : AstProgItem(Kind::Function)
        , name(name)
        , retType(retType)
        , params(params)
        , block(block) {  }

    StringRef getName() const { return name; }
    SymbolType* getReturnType() const { return retType; }
    const BoundArray<AstFunParam*>& getParams() const { return params; }

    AstBlock* getBlock() { return block; }
    const AstBlock* getBlock() const { return block; }

private:
    StringRef name;
    SymbolType* retType;
    BoundArray<AstFunParam*> params;
    AstBlock* block;
};
