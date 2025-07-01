#pragma once
#include "SkrValue.h"

class SkrExpRes {
public:
    enum class Kind { Plain, DereferencedPtr };

    static SkrExpRes plain(SkrValue* value) { return SkrExpRes(value, Kind::Plain); }
    static SkrExpRes dereferenced(SkrValue* value) { return SkrExpRes(value, Kind::DereferencedPtr); }

    SkrValue* get() const { return val; }

    const Kind kind;

private:
    SkrExpRes(SkrValue* val, Kind kind) : val(val), kind(kind) {  }

    SkrValue* val;
};
