#pragma once
#include "SkrInstruction.h"

class SkrOffsetLoad : public SkrInstruction {
public:
    SkrOffsetLoad(SkrValue* to, SkrValue* from, int fromOffset) 
        : SkrInstruction(Kind::OffsetLoad)
        , to(to) 
        , from(from)
        , fromOffset(fromOffset) { }

    SkrValue* getFrom() const { return from; }
    int getFromOffset() const { return fromOffset; }
    SkrValue* getTo() const { return to; }

private:
    SkrValue* to;
    SkrValue* from;
    int fromOffset;
};
