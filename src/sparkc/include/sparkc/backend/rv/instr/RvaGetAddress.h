#pragma once
#include "RvaInstruction.h"

class RvaGetAddress : public RvaInstruction {
public:
    RvaGetAddress(RvaValue* to, RvaValue* of);

    void emit(RvListing& listing) override;

    RvaValue* to;
    RvaValue* of;
};
