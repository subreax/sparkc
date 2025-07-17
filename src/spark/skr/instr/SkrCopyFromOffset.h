#pragma once
#include "SkrInstruction.h"

class SkrCopyFromOffset : public SkrInstruction {
public:
    SkrCopyFromOffset(SkrValue* to, SkrValue* from, int fromOffset)
        : SkrInstruction(Kind::CopyFromOffset)
        , to(to)
        , from(from)
        , fromOffset(fromOffset) {  }
    
    SkrValue* getTo() const { return to; }
    SkrValue* getFrom() const { return from; }
    int getFromOffset() const { return fromOffset; }

private:
    SkrValue* to;
    SkrValue* from;
    int fromOffset;
};
