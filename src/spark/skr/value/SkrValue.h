#pragma once
#include <cstdint>
#include "../../common/Constant.h"

class SkrConst;
class SkrVar;

class SkrValue {
public:
    enum class Kind { Const, Var };

    SkrValue(Kind kind) : kind(kind) {  }

    bool isConst() const { return kind == Kind::Const; }
    bool isVar() const { return kind == Kind::Var; }

    SkrConst* toSkrConst() {
        return (SkrConst*) this;
    }

    const SkrConst* toSkrConst() const {
        return (const SkrConst*) this;
    }

    SkrVar* toSkrVar() {
        return (SkrVar*) this;
    }

    const Kind kind;
};


class SkrConst : public SkrValue {
public:
    SkrConst(Constant* c) : SkrValue(Kind::Const), c(c) { }

    Constant* getConst() const { return c; }

private:
    Constant* c;
};


class SkrVar : public SkrValue {
public:
    SkrVar(const char* id) : SkrValue(Kind::Var), id(id) {  }

    const char* getId() const { return id; }

private:
    const char* id;
};
