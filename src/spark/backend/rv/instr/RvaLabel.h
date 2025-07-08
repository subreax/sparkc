#pragma once
#include "RvaInstruction.h"

class RvaLabel : public RvaInstruction {
public:
    RvaLabel(StringRef value) : RvaInstruction(Kind::Label), value(value) {  }

    void emit(RvListing& listing) override {
        listing.addLabel(value);
    }

    StringRef getValue() const { return value; }

private:
    StringRef value;
};