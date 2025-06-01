#pragma once
#include "SkrInstruction.h"

class SkrInt2Float : public SkrInstruction {
public:
    SkrInt2Float(SkrValue* dst, SkrValue* src)
        : SkrInstruction(Kind::Int2Float)
        , dst(dst)
        , src(src) {  }

    SkrValue* getDst() { return dst; }
    SkrValue* getSrc() { return src; }

private:
    SkrValue* dst;
    SkrValue* src;
};