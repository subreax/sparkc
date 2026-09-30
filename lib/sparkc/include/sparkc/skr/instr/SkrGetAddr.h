#pragma once
#include "SkrInstruction.h"

class SkrGetAddr : public SkrInstruction {
public:
    SkrGetAddr(SkrVar* to, SkrVar* var, int offset)
        : SkrInstruction(Kind::GetAddr)
        , to(to)
        , var(var)
        , offset(offset) { }

    SkrVar* getTo() const { return to; }
    SkrVar* getVar() const { return var; }
    int getOffset() const { return offset; }

private:
    SkrVar* to;
    SkrVar* var;
    int offset;
};