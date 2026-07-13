#pragma once
#include "SkrInstruction.h"

class SkrFloat2Int : public SkrInstruction {
public:
    SkrFloat2Int(SkrVar* dst, SkrValue* src)
        : SkrInstruction(Kind::Float2Int)
        , dst(dst)
        , src(src) { }

    SkrVar* getDst() { return dst; }
    const SkrVar* getDst() const { return dst; }

    SkrValue* getSrc() { return src; }
    const SkrValue* getSrc() const { return src; }

private:
    SkrVar* dst;
    SkrValue* src;
};
