#pragma once
#include "RvaInstruction.h"

class RvaReserveOnStack : public RvaInstruction {
public:
    RvaReserveOnStack(RvaValue* mem);

    void emit(RvListing& listing) override;

    RvaValue* mem;
};