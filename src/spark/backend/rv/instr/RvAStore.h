#pragma once
#include "RvaInstruction.h"

class RvaStore : public RvaInstruction {
public:
    RvaStore(RvaValue* toAddr, RvaValue* from) 
        : RvaInstruction(Type::Store)
        , toAddr(toAddr)
        , from(from) { }
    
    void emit(RvListing& listing) override {
        auto* mem = expectMem(toAddr);
        listing += Rv32I::sw(mem->getBase(), mem->getOffset(), expectReg(from));
    }

    RvaValue* toAddr;
    RvaValue* from;
};