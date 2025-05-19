#pragma once
#include "AstFunction.h"

struct AstProgram {
    std::vector<AstFunction*> functions;
};
