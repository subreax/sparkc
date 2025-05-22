#pragma once
#include "RvaInstruction.h"

class RvaJump : public RvaInstruction {
public:
    RvaJump(const char* label) : RvaInstruction(Type::Jump), label(label) {  }

    void emit(RvListing& listing) override {
        listing.addWithLabel(Rv32I::jal(RvReg::ZERO), label);
    }

    const char* getLabel() const { return label; }

private:
    const char* label;
};