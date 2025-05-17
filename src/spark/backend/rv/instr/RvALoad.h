#pragma once
#include "RvAInstruction.h"

class RvALoad : public RvAInstruction {
public:
    RvALoad(RvAValue* to, RvAValue* fromAddr) 
        : RvAInstruction(Type::Load)
        , to(to)
        , fromAddr(fromAddr) {}

    void emit(RvListing& listing) override {}

    RvAValue* to;
    RvAValue* fromAddr;
};