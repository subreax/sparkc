#pragma once
#include "RvaInstruction.h"

class RvaStore : public RvaInstruction {
public:
    RvaStore(RvaMemory* to, RvaValue* from);

    void emit(RvListing& listing) override;

    RvaMemory* to;
    RvaValue* from;
};