#pragma once
#include "SkrInstruction.h"

class SkrStore : public SkrInstruction {
public:
    SkrStore(SkrValue* to, SkrValue* from) : SkrInstruction(Kind::Store), to(to), from(from) {  }

    SkrValue* getTo() const { return to; }
    SkrValue* getFrom() const { return from; }

private:
    SkrValue* to;
    SkrValue* from;
};