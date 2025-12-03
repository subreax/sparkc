#pragma once
#include "SkrInstruction.h"

class SkrInt2Float : public SkrInstruction {
public:
    SkrInt2Float(SkrVar* dst, SkrValue* src)
        : SkrInstruction(Kind::Int2Float)
        , dst(dst)
        , src(src) {  }

    SkrVar* getDst() { return dst; }
    const SkrVar* getDst() const { return dst; }

    SkrValue* getSrc() { return src; }
    const SkrValue* getSrc() const { return src; }

private:
    SkrVar* dst;
    SkrValue* src;
};