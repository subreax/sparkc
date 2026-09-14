#pragma once
#include "RvaInstruction.h"

class RvaDataAlloc : public RvaInstruction {
public:
    RvaDataAlloc(StringRef label, size_t sz)
        : RvaInstruction(Kind::DataAlloc)
        , label(label)
        , sz(sz) { }

    StringRef getLabel() const {
        return label;
    }

    size_t getSize() const {
        return sz;
    }

    void emit(RvListing& listing) override;

private:
    StringRef label;
    size_t sz;
};