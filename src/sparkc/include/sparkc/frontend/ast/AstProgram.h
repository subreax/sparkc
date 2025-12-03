#pragma once
#include "sparkc/common/BoundArray.h"
#include "AstProgItem.h"

struct AstProgram {
    AstProgram(const BoundArray<AstProgItem*>& items) : items(items) {  }

    BoundArray<AstProgItem*> items;
};
