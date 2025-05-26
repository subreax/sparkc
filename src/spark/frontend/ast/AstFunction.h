#pragma once
#include <vector>
#include "AstBlockItem.h"
#include "AstFunParam.h"

class AstFunction {
public:
    AstFunction(const char* name, const std::vector<AstFunParam*> params, const std::vector<AstBlockItem*>& blockItems)
        : name(name)
        , params(params)
        , blockItems(blockItems) {  }

    const char* getName() const { return name; }
    const std::vector<AstFunParam*>& getParams() const { return params; }
    const std::vector<AstBlockItem*>& getBlockItems() const { return blockItems; }

private:
    const char* name;
    std::vector<AstFunParam*> params;
    std::vector<AstBlockItem*> blockItems;
};
