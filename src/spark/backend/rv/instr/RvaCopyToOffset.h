#pragma once
#include "RvaInstruction.h"

class RvaCopyToOffset : public RvaInstruction {
public:
    RvaCopyToOffset(RvaValue* dst, int dstOffset, RvaValue* src) 
        : RvaInstruction(Kind::CopyToOffset) 
        , dst(dst), src(src), dstOffset(dstOffset) {  }

    void emit(RvListing& listing) override {
        listing.add(Rv32I::sw(expectReg(dst), dstOffset, expectReg(src)));
    }

    RvaValue* dst;
    int dstOffset;
    RvaValue* src;
};
