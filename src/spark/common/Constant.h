#pragma once
#include <cstdint>
#include "../symbol/SymbolType.h"
#include "Error.h"

class IntConstant;
class FloatConstant;

class Constant {
public:
    Constant(SymbolType* type) : type(type) {  }

    bool isInt() const { return type->kind == SymbolType::Kind::Integer; }
    bool isFloat() const { return type->kind == SymbolType::Kind::Float; }

    int32_t intValue() const;
    float floatValue() const;

    bool operator==(const Constant& other) const;

    SymbolType *const type;
};

class IntConstant : public Constant {
public:
    IntConstant(int32_t val) 
        : Constant(SymbolIntType::getInstance()), val(val) {  }

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
        : Constant(SymbolFloatType::getInstance()), val(val) {  }

    float val;
};
