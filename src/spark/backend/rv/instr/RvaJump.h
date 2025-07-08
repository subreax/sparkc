#pragma once
#include "RvaInstruction.h"

class RvaJump : public RvaInstruction {
public:
    RvaJump(StringRef label) : RvaInstruction(Kind::Jump), label(label) {  }

    void emit(RvListing& listing) override {
        listing.addWithLabel(Rv32I::jal(RvReg::ZERO), label);
    }

    StringRef getLabel() const { return label; }

private:
    StringRef label;
};