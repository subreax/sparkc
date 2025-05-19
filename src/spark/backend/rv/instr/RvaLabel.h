#pragma once
#include "RvaInstruction.h"

class RvaLabel : public RvaInstruction {
public:
    RvaLabel(const char* value) : RvaInstruction(Type::Label), value(value) {  }

    void emit(RvListing& listing) override {}

    const char* getValue() const { return value; }

private:
    const char* value;
};