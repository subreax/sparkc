#pragma once
#include "AstDeclaration.h"
#include "../exp/AstExp.h"

class AstVarDeclaration : public AstDeclaration {
public:
    AstVarDeclaration(StringRef id, SymbolType* type, AstExp* init = nullptr)
        : AstDeclaration(Kind::Var)
        , id(id)
        , type(type)
        , init(init) { }

    StringRef getId() const { return id; }
    void setId(StringRef id) { this->id = id; }
    
    AstExp* getInitializer() { return init; }
    void setInitializer(AstExp* init) { this->init = init; }

    SymbolType* getType() { return type; }

private:
    StringRef id;
    SymbolType* type;
    AstExp* init;
};