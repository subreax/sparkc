#pragma once
#include "../common/BoundArray.h"

class SymbolType {
public:
    enum class Kind { Integer, Float, Pointer, Function };

    SymbolType(Kind kind) : kind(kind) {  }

    const Kind kind;
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

class SymbolFloatType : public SymbolType {
public:
    static SymbolFloatType* getInstance() {
        static SymbolFloatType it;
        return &it;
    }

private:
    SymbolFloatType() : SymbolType(Kind::Float) {  }
};

class SymbolFunctionType : public SymbolType {
public:
    SymbolFunctionType(SymbolType* retType, BoundArray<SymbolType*> params) 
        : SymbolType(Kind::Function)
        , params(params)
        , retType(retType) {  }

    BoundArray<SymbolType*> getParams() { return params; }
    const BoundArray<SymbolType*> getParams() const { return params; }

    SymbolType* getReturnType() const { return retType; }

private:
    BoundArray<SymbolType*> params;
    SymbolType* retType;
};

class SymbolPointerType : public SymbolType {
public:
    SymbolPointerType(SymbolType* varType) : SymbolType(Kind::Pointer), varType(varType) {  }

    SymbolType* getVarType() const { return varType; }

private:
    SymbolType* varType;
};
