#pragma once
#include <cstdint>

class SkrValue {
public:
    enum class Type { Const, Var };

    SkrValue(Type type) : type(type) {  }

    Type getType() const { return type; }

private:
    Type type;
};


class SkrConst : public SkrValue {
public:
    SkrConst(int32_t c) : SkrValue(Type::Const), c(c) { }

    int32_t getConst() const { return c; }

private:
    int32_t c;
};


class SkrVar : public SkrValue {
public:
    SkrVar(const char* identifier) : SkrValue(Type::Var), identifier(identifier) {  }

    const char* getIdentifier() const { return identifier; }

private:
    const char* identifier;
};
