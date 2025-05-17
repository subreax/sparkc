#pragma once
#include "RvaInstruction.h"

class RvaStore : public RvaInstruction {
public:
    RvaStore(RvaValue* toAddr, RvaValue* from) 
        : RvaInstruction(Type::Store)
        , toAddr(toAddr)
        , from(from) { }
    
    void emit(RvListing& listing) override {}

    RvaValue* toAddr;
    RvaValue* from;
};