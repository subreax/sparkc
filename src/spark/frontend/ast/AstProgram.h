#pragma once
#include "AstFunction.h"

struct AstProgram {
    AstProgram(BoundArray<AstFunction*> functions) : functions(functions) {  }

    BoundArray<AstFunction*> functions;
};
