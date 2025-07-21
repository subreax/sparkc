#pragma once
#include "RvAInstruction.h"

class RvaEndTempStack : public RvaInstruction {
public:
    RvaEndTempStack() : RvaInstruction(Kind::EndTempStack) { }

    void emit(RvListing& listing) override {
        sparkError("RvaEndTempStack", "emit() shouldn't be called");
    }
};
