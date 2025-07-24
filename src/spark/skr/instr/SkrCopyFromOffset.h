#pragma once
#include "SkrInstruction.h"

class SkrCopyFromOffset : public SkrInstruction {
public:
    SkrCopyFromOffset(SkrVar* to, SkrValue* from, int fromOffset)
        : SkrInstruction(Kind::CopyFromOffset)
        , to(to)
        , from(from)
        , fromOffset(fromOffset) {  }
    
    SkrVar* getTo() const { return to; }
    SkrValue* getFrom() const { return from; }
    int getFromOffset() const { return fromOffset; }

private:
    SkrVar* to;
    SkrValue* from;
    int fromOffset;
};
