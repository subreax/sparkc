#pragma once
#include <cstdint>
#include "../symbol/SymbolType.h"
#include "Error.h"

class Constant {
public:
    enum class Kind { Int, Float };

    Constant(Kind kind) : kind(kind) {  }

    SymbolType* getType() {
        if (kind == Kind::Int) return SymbolIntType::getInstance();
        if (kind == Kind::Float) return SymbolFloatType::getInstance();

        sparkError("Constant", "getType failed, unknown kind: %d", kind);
        return nullptr;
    }

    const Kind kind;
};

class IntConstant : public Constant {
public:
    IntConstant(int32_t val) 
        : Constant(Kind::Int), val(val) {  }

    static IntConstant* get0() {
        static IntConstant c(0);
        return &c;
    }

    static IntConstant* get1() {
        static IntConstant c(1);
        return &c;
    }

    int32_t val;
};

class FloatConstant : public Constant {
public:
    FloatConstant(float val) 
        : Constant(Kind::Float), val(val) {  }

    float val;
};
