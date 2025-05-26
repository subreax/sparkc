#pragma once
#include "RvaInstruction.h"

class RvaLabel : public RvaInstruction {
public:
    RvaLabel(const char* value) : RvaInstruction(Kind::Label), value(value) {  }

    void emit(RvListing& listing) override {
        listing.addLabel(value);
    }

    const char* getValue() const { return value; }

private:
    const char* value;
};