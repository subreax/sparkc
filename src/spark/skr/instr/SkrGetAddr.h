#pragma once
#include "SkrInstruction.h"

class SkrGetAddr : public SkrInstruction {
public:
    SkrGetAddr(SkrVar* to, SkrVar* var)
        : SkrInstruction(Kind::GetAddr)
        , to(to)
        , var(var) {  }

    SkrVar* getTo() const { return to; }
    SkrVar* getVar() const { return var; }

private:
    SkrVar* to;
    SkrVar* var;
};