#pragma once
#include "SymbolType.h"

class Symbol {
public:
    Symbol() = default;

    Symbol(SymbolType* type, bool isStatic)
        : type(type)
        , _isStatic(isStatic) { }

    SymbolType* getType() {
        return type;
    }

    const SymbolType* getType() const {
        return type;
    }

    bool isStatic() const {
        return _isStatic;
    }

private:
    SymbolType* type = nullptr;
    bool _isStatic = false;
};