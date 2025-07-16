#pragma once
#include "RvaInstruction.h"

class RvaLoad : public RvaInstruction {
public:
    RvaLoad(RvaValue* to, RvaMemory* from) 
        : RvaInstruction(Kind::Load)
        , to(to)
        , from(from) {}

    void emit(RvListing& listing) override {
        listing += Rv32I::lw(expectReg(to), from->getBase(), from->getOffset());
    }

    RvaValue* to;
    RvaMemory* from;
};