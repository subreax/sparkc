#pragma once
#include "RvaInstruction.h"

class RvaRet : public RvaInstruction {
public:
    RvaRet() : RvaInstruction(Kind::Ret) { }

    void emit(RvListing& listing) override {
        listing += Rv32I::ret();
    }
};