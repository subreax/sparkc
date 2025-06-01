#pragma once
#include "exp/AstExp.h"

class AstFunParam {
public:
    AstFunParam(const char* id, SymbolType* type) : id(id), type(type) {  }

    const char* getIdentifier() const { return id; }
    SymbolType* getType() const { return type; }

private:
    const char* id;
    SymbolType* type;
};
