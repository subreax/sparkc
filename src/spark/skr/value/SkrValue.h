#pragma once
#include <cstdint>

class SkrValue {
public:
    enum class Kind { Const, Var };

    SkrValue(Kind kind) : kind(kind) {  }

    const Kind kind;
};


class SkrConst : public SkrValue {
public:
    SkrConst(int32_t c) : SkrValue(Kind::Const), c(c) { }

    int32_t getConst() const { return c; }

private:
    int32_t c;
};


class SkrVar : public SkrValue {
public:
    SkrVar(const char* id) : SkrValue(Kind::Var), id(id) {  }

    const char* getId() const { return id; }

private:
    const char* id;
};
