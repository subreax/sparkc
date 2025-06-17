#pragma once
#include "SkrInstruction.h"
#include "../value/SkrValue.h"

class SkrCopy : public SkrInstruction {
public:
    SkrCopy(SkrValue* to, SkrValue* from) 
        : SkrInstruction(Kind::Copy)
        , to(to)
        , from(from) {  }

    SkrValue* getFrom() { return from; }
    SkrValue* getTo() { return to; }

    bool operator==(const SkrCopy& other) const {
        return *to == *other.to && *from == *other.from;
    }

    bool operator!=(const SkrCopy& other) const {
        return *to != *other.to || *from != *other.from;
    }

private:
    SkrValue* to;
    SkrValue* from;
};