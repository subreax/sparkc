#pragma once
#include "SkrValue.h"

class SkrExpRes {
public:
    enum class Kind { Val, Ptr, Field };

    static SkrExpRes val(SkrValue* value, SymbolType* type) { return SkrExpRes(Kind::Val, value, type, 0); }
    static SkrExpRes ptr(SkrValue* value, SymbolType* type) { return SkrExpRes(Kind::Ptr, value, type, 0); }
    static SkrExpRes field(SkrValue* base, int offset, SymbolType* type) { return SkrExpRes(Kind::Field, base, type, offset); }

    SkrValue* get() const { return _val; }
    SymbolType* getType() const { return _type; }
    int getOffset() const { return _offset; }

    const Kind kind;

private:
    SkrExpRes(Kind kind, SkrValue* val, SymbolType* type, int offset) : kind(kind), _val(val), _type(type), _offset(offset) {  }

    SkrValue* _val;
    SymbolType* _type;
    int _offset;
};
