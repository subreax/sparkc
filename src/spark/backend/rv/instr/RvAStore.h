#pragma once
#include "RvaInstruction.h"

class RvaStore : public RvaInstruction {
public:
    RvaStore(RvaMemory* to, RvaValue* from) 
        : RvaInstruction(Kind::Store)
        , to(to)
        , from(from) { }
    
    void emit(RvListing& listing) override {
        listing += Rv32I::sw(to->getBase(), to->getOffset(), expectReg(from));
    }

    RvaMemory* to;
    RvaValue* from;
};