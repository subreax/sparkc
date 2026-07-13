#pragma once
#include "RvaInstruction.h"

class RvaLoad : public RvaInstruction {
public:
    RvaLoad(RvaValue* to, RvaMemory* from);

    void emit(RvListing& listing) override;

    RvaValue* to;
    RvaMemory* from;
};