#pragma once
#include "SkrValue.h"

class SkrExpRes {
public:
    enum class Kind { Val, /* Ptr, */ Field };

    static SkrExpRes val(SkrValue* value) { return SkrExpRes(Kind::Val, value, 0); }
    // static SkrExpRes ptr(SkrValue* base, int offset) { return SkrExpRes(Kind::Ptr, base, offset); }
    static SkrExpRes field(SkrValue* base, int offset) { return SkrExpRes(Kind::Field, base, offset); }

    SkrValue* get() const { return _val; }

    SkrVar* getBase() const {
        if (_val->isVar()) {
            return _val->toSkrVar();
        }
        sparkError("SkrExpRes", "getBase(): _val is not a SkrVar");
        return nullptr;
    }

    int getOffset() const { return _offset; }

    const Kind kind;

private:
    SkrExpRes(Kind kind, SkrValue* val, int offset) : kind(kind), _val(val), _offset(offset) {  }

    SkrValue* _val;
    int _offset;
};
