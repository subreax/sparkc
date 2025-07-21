#pragma once
#include "RvAInstruction.h"

class RvaBeginTempStack : public RvaInstruction {
public:
    RvaBeginTempStack() : RvaInstruction(Kind::BeginTempStack) { }

    void emit(RvListing& listing) override {
        sparkError("RvaBeginTempStack", "emit() shouldn't be called");
    }
};
