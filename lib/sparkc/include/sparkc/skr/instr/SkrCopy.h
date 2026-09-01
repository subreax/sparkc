#pragma once
#include "../value/SkrValue.h"
#include "SkrInstruction.h"

class SkrCopy : public SkrInstruction {
public:
    SkrCopy(SkrVar* to, SkrValue* from)
        : SkrInstruction(Kind::Copy)
        , to(to)
        , from(from) { }

    SkrValue* getFrom() { return from; }
    const SkrValue* getFrom() const { return from; }

    SkrVar* getTo() { return to; }
    const SkrVar* getTo() const { return to; }

    bool operator==(const SkrCopy& other) const {
        return *to == *other.to && *from == *other.from;
    }

    bool operator!=(const SkrCopy& other) const {
        return *to != *other.to || *from != *other.from;
    }

private:
    SkrVar* to;
    SkrValue* from;
};