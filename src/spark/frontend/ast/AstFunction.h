#pragma once
#include <vector>
#include "AstBlockItem.h"
#include "AstFunParam.h"
#include "../../common/BoundArray.h"

class AstFunction {
public:
    AstFunction(const char* name, BoundArray<AstFunParam*> params, BoundArray<AstBlockItem*> blockItems)
        : name(name)
        , params(params)
        , blockItems(blockItems) {  }

    const char* getName() const { return name; }
    const BoundArray<AstFunParam*>& getParams() const { return params; }
    const BoundArray<AstBlockItem*>& getBlockItems() const { return blockItems; }

private:
    const char* name;
    BoundArray<AstFunParam*> params;
    BoundArray<AstBlockItem*> blockItems;
};
