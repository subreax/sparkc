#pragma once
#include "RvaInstruction.h"

class RvaLabel : public RvaInstruction {
public:
    RvaLabel(StringRef value);

    void emit(RvListing& listing) override;

    StringRef getValue() const;

private:
    StringRef value;
};