#pragma once
#include "RvaInstruction.h"

class RvaJump : public RvaInstruction {
public:
    RvaJump(StringRef label);

    void emit(RvListing& listing) override;

    StringRef getLabel() const;

private:
    StringRef label;
};