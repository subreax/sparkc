#pragma once
#include "exp/AstExp.h"

class AstFunParam {
public:
    AstFunParam(StringRef id, SymbolType* type)
        : id(id)
        , type(type) { }

    StringRef getId() const { return id; }
    void setId(StringRef id) { this->id = id; }

    SymbolType* getType() const { return type; }

private:
    StringRef id;
    SymbolType* type;
};
