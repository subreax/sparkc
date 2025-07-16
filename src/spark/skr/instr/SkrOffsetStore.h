#pragma once
#include "SkrInstruction.h"

class SkrOffsetStore : public SkrInstruction {
public:
    SkrOffsetStore(SkrVar* to, int toOffset, SkrValue* from)
        : SkrInstruction(Kind::OffsetStore)
        , to(to)
        , toOffset(toOffset)
        , from(from) {  }

    SkrVar* getTo() const { return to; }
    int getToOffset() const { return toOffset; }
    SkrValue* getFrom() const { return from; }

private:
    SkrVar* to;
    int toOffset;
    SkrValue* from;
};
