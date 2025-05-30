#pragma once
#include "../../common/BoundArray.h"
#include "AstBlockItem.h"

class AstBlock {
public:
    AstBlock(BoundArray<AstBlockItem*> items) : items(items) {  }

    const BoundArray<AstBlockItem*>& getItems() const { return items; }

private:
    BoundArray<AstBlockItem*> items;
};