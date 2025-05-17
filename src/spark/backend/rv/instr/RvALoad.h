#pragma once
#include "RvaInstruction.h"

class RvaLoad : public RvaInstruction {
public:
    RvaLoad(RvaValue* to, RvaValue* fromAddr) 
        : RvaInstruction(Type::Load)
        , to(to)
        , fromAddr(fromAddr) {}

    void emit(RvListing& listing) override {}

    RvaValue* to;
    RvaValue* fromAddr;
};