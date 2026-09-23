#pragma once
#include "RvaInstruction.h"

class RvaDataCreateString : public RvaInstruction {
public:
    RvaDataCreateString(StringRef label, StringRef str)
        : RvaInstruction(Kind::DataCreateString)
        , label(label)
        , str(str) { }

    StringRef getLabel() const { return label; }
    StringRef getString() const { return str; }

    void emit(RvListing& listing) override;

private:
    StringRef label;
    StringRef str;
};