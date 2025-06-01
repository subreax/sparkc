#pragma once
#include "SkrInstruction.h"

class SkrFloat2Int : public SkrInstruction {
public:
    SkrFloat2Int(SkrValue* dst, SkrValue* src)
        : SkrInstruction(Kind::Float2Int)
        , dst(dst)
        , src(src) {  }

    SkrValue* getDst() { return dst; }
    SkrValue* getSrc() { return src; }

private:
    SkrValue* dst;
    SkrValue* src;
};
