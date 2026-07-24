#pragma once
#include "AstProgItem.h"
#include "AstBlock.h"
#include "sparkc/common/BoundArray.h"
#include "sparkc/symbol/SymbolType.h"
#include <vector>

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

class AstFunction : public AstProgItem {
public:
    AstFunction(StringRef name, SymbolType* retType, BoundArray<AstFunParam*> params, AstBlock* block)
        : AstProgItem(Kind::Function)
        , name(name)
        , retType(retType)
        , params(params)
        , block(block) { }

    StringRef getName() const { return name; }
    SymbolType* getReturnType() const { return retType; }
    const BoundArray<AstFunParam*>& getParams() const { return params; }

    AstBlock* getBlock() { return block; }
    const AstBlock* getBlock() const { return block; }

private:
    StringRef name;
    SymbolType* retType;
    BoundArray<AstFunParam*> params;
    AstBlock* block;
};
