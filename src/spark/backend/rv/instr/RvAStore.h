#pragma once
#include "RvAInstruction.h"

class RvAStore : public RvAInstruction {
public:
    RvAStore(RvAValue* toAddr, RvAValue* from) 
        : RvAInstruction(Type::Store)
        , toAddr(toAddr)
        , from(from) { }
    
    void emit(RvListing& listing) override {}

    RvAValue* toAddr;
    RvAValue* from;
};