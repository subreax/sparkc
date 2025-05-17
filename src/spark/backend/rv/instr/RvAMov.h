#pragma once
#include "RvaInstruction.h"

class RvaMov : public RvaInstruction {
public:
    RvaMov(RvaValue* to, RvaValue* from) 
        : RvaInstruction(Type::Move)
        , to(to)
        , from(from) {}

    void emit(RvListing& listing) override {}

    RvaValue* to;
    RvaValue* from;
};
