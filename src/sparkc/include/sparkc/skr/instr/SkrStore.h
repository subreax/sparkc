#pragma once
#include "SkrInstruction.h"

class SkrStore : public SkrInstruction {
public:
    SkrStore(SkrVar* to, int toOffset, SkrValue* from)
        : SkrInstruction(Kind::Store)
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