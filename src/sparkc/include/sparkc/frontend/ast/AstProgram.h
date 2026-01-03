#pragma once
#include "AstProgItem.h"
#include "sparkc/common/BoundArray.h"

struct AstProgram {
    AstProgram(const BoundArray<AstProgItem*>& items)
        : items(items) { }

    BoundArray<AstProgItem*> items;
};
