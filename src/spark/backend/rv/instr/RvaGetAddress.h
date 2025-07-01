#pragma once
#include "RvaInstruction.h"

class RvaGetAddress : public RvaInstruction {
public:
    RvaGetAddress(RvaValue* to, RvaValue* of) : RvaInstruction(Kind::GetAddress), to(to), of(of) {  }

    void emit(RvListing& listing) override {
        sparkError("RvaGetAddress", "emit shouldn't be called");
    }

    RvaValue* to;
    RvaValue* of;
};
