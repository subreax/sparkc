#pragma once
#include "SkrInstruction.h"

class SkrLoad : public SkrInstruction {
public:
    SkrLoad(SkrValue* to, SkrValue* from) : SkrInstruction(Kind::Load), to(to), from(from) {  }

    SkrValue* getTo() const { return to; }
    SkrValue* getFrom() const { return from; }

private:
    SkrValue* to;
    SkrValue* from;
};