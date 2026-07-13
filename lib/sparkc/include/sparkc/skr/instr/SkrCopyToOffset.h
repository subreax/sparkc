#pragma once
#include "SkrInstruction.h"

class SkrCopyToOffset : public SkrInstruction {
public:
    SkrCopyToOffset(SkrVar* to, int toOffset, SkrValue* from)
        : SkrInstruction(Kind::CopyToOffset)
        , to(to)
        , toOffset(toOffset)
        , from(from) { }

    SkrVar* getTo() const { return to; }
    int getToOffset() const { return toOffset; }
    SkrValue* getFrom() const { return from; }

private:
    SkrVar* to;
    int toOffset;
    SkrValue* from;
};
