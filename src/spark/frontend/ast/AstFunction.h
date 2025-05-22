#pragma once
#include <vector>
#include "AstBlockItem.h"

class AstFunction {
public:
    AstFunction(const char* name, const std::vector<AstBlockItem*>& blockItems)
        : name(name), blockItems(blockItems) {  }

    const char* getName() const { return name; }
    const std::vector<AstBlockItem*>& getBlockItems() { return blockItems; }

private:
    const char* name;
    std::vector<AstBlockItem*> blockItems;
};
