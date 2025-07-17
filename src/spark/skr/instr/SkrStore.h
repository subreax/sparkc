#pragma once
#include "SkrInstruction.h"

class SkrStore : public SkrInstruction {
public:
    SkrStore(SkrValue* to, int toOffset, SkrValue* from) 
        : SkrInstruction(Kind::Store)
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