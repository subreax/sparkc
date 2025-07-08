#pragma once
#include "SkrInstruction.h"

class SkrOffsetStore : public SkrInstruction {
public:
    SkrOffsetStore(SkrValue* to, int toOffset, SkrValue* from)
        : SkrInstruction(Kind::OffsetStore)
        , to(to)
        , toOffset(toOffset)
        , from(from) {  }

    SkrValue* getTo() const { return to; }
    int getToOffset() const { return toOffset; }
    SkrValue* getFrom() const { return from; }

private:
    SkrValue* to;
    int toOffset;
    SkrValue* from;
};
