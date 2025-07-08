#pragma once
#include "SkrValue.h"

class SkrExpRes {
public:
    enum class Kind { Val, Ptr, Field };

    static SkrExpRes val(SkrValue* value) { return SkrExpRes(Kind::Val, value, 0); }
    static SkrExpRes ptr(SkrValue* value) { return SkrExpRes(Kind::Ptr, value, 0); }
    static SkrExpRes field(SkrValue* base, int offset) { return SkrExpRes(Kind::Field, base, offset); }

    SkrValue* get() const { return _val; }
    SkrValue* getBase() const { return _val; }
    int getOffset() const { return _offset; }

    const Kind kind;

private:
    SkrExpRes(Kind kind, SkrValue* val, int offset) : kind(kind), _val(val), _offset(offset) {  }

    SkrValue* _val;
    int _offset;
};
