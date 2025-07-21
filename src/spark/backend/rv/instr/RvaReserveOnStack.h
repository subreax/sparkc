#pragma once
#include "RvaInstruction.h"

class RvaReserveOnStack : public RvaInstruction {
public:
    RvaReserveOnStack(RvaValue* mem) 
        : RvaInstruction(Kind::ReserveOnStack)
        , mem(mem) {  }

    void emit(RvListing& listing) override {
        sparkError("RvaReserveOnStack", "emit() shouldn't be called");
    }

    RvaValue* mem;
};