#pragma once
#include "AstBlockItem.h"
#include "sparkc/common/BoundArray.h"

class AstBlock {
public:
    AstBlock(BoundArray<AstBlockItem*> items)
        : items(items) { }

    const BoundArray<AstBlockItem*>& getItems() const { return items; }

private:
    BoundArray<AstBlockItem*> items;
};