#pragma once
#include "RvaInstruction.h"

class RvaBeginTempStack : public RvaInstruction {
public:
    RvaBeginTempStack() : RvaInstruction(Kind::BeginTempStack) { }

    void emit(RvListing& listing) override {
        sparkError("RvaBeginTempStack", "emit() shouldn't be called");
    }
};
