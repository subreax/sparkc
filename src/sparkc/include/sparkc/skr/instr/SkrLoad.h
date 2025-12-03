#pragma once
#include "SkrInstruction.h"

class SkrLoad : public SkrInstruction {
public:
    SkrLoad(SkrValue* to, SkrValue* from, int fromOffset) 
        : SkrInstruction(Kind::Load)
        , to(to)
        , fromOffset(fromOffset)
        , from(from) {  }

    SkrValue* getTo() const { return to; }
    int getFromOffset() const { return fromOffset; }
    SkrValue* getFrom() const { return from; }

private:
    SkrValue* to;
    SkrValue* from;
    int fromOffset;
};