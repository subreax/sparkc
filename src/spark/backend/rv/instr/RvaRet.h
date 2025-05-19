#pragma once
#include "RvaInstruction.h"

class RvaRet : public RvaInstruction {
public:
    RvaRet() : RvaInstruction(Type::Ret) { }

    void emit(RvListing& listing) override { }
};