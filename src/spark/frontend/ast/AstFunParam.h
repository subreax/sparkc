#pragma once
#include "exp/AstExp.h"

class AstFunParam {
public:
    AstFunParam(const char* id) : id(id) {  }

    const char* getIdentifier() const { return id; }

private:
    const char* id;
};
