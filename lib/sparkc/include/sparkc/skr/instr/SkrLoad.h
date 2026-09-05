#pragma once
#include "SkrInstruction.h"

class SkrLoad : public SkrInstruction {
public:
    SkrLoad(SkrValue* to, SkrValue* from, int fromOffset)
        : SkrInstruction(Kind::Load)
        , to(to)
        , from(from) 
        , fromOffset(fromOffset)
        { }

    SkrValue* getTo() const { return to; }
    int getFromOffset() const { return fromOffset; }
    SkrValue* getFrom() const { return from; }

private:
    SkrValue* to;
    SkrValue* from;
    int fromOffset;
};