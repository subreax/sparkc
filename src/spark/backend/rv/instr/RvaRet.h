#pragma once
#include "RvaInstruction.h"

class RvaRet : public RvaInstruction {
public:
    RvaRet() : RvaInstruction(Type::Ret) { }

    void emit(RvListing& listing) override {
        listing += Rv32I::jalr(RvReg::ZERO, RvReg::RA, 0);
    }
};