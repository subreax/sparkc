#pragma once
#include "RvAInstruction.h"

class RvAMov : public RvAInstruction {
public:
    RvAMov(RvAValue* to, RvAValue* from) 
        : RvAInstruction(Type::Move)
        , to(to)
        , from(from) {}

    void emit(RvListing& listing) override {}

    RvAValue* to;
    RvAValue* from;
};
