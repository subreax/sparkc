#pragma once
#include "RvaInstruction.h"

class RvaBeginTempStack : public RvaInstruction {
public:
    RvaBeginTempStack();
    void emit(RvListing& listing) override;
};
