#pragma once
#include "../BoundArray.h"

class SymbolType {
public:
    enum class Kind { Integer, Function };

    SymbolType(Kind kind) : kind(kind) {  }

    Kind getKind() const { return kind; }

private:
    Kind kind;
};


class SymbolIntType : public SymbolType {
public:
    static SymbolIntType* getInstance() {
        static SymbolIntType it;
        return &it;
    }

private:
    SymbolIntType() : SymbolType(Kind::Integer) {  }
};


class SymbolFunctionType : public SymbolType {
public:
    SymbolFunctionType(SymbolType* retType, BoundArray<SymbolType*> params) 
        : SymbolType(Kind::Function)
        , params(params) {  }

    BoundArray<SymbolType*> getParams() { return params; }
    SymbolType* getReturnType() { return retType; }

private:
    BoundArray<SymbolType*> params;
    SymbolType* retType;
};
