#pragma once
#include "RvaInstruction.h"

class RvaRet : public RvaInstruction {
public:
    RvaRet();

    void emit(RvListing& listing) override;
};