#pragma once
#include "AstDeclaration.h"
#include "../exp/AstExp.h"

class AstVarDeclaration : public AstDeclaration {
public:
    AstVarDeclaration(const char* name, SymbolType* type, AstExp* init = nullptr)
        : AstDeclaration(Kind::Var)
        , name(name)
        , init(init) { }

    const char* getName() const { return name; }
    AstExp* getInitializer() { return init; }
    SymbolType* getType() { return type; }

private:
    const char* name;
    AstExp* init;
    SymbolType* type;
};