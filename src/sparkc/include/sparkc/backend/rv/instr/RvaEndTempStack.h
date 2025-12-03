#pragma once
#include "RvaInstruction.h"

class RvaEndTempStack : public RvaInstruction {
public:
    RvaEndTempStack();
    void emit(RvListing& listing) override;
};
