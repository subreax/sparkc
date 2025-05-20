#pragma once
#include "RvaInstruction.h"

class RvaEpilogue : public RvaInstruction {
public:
    RvaEpilogue(int frameSize) : RvaInstruction(Type::Epilogue), frameSize(frameSize) {  }

    int getFrameSize() const { return frameSize; }
    void setFrameSize(int size) { frameSize = size; }

    void emit(RvListing& listing) override {
        if (frameSize == 0) return;

        listing += Rv32I::lw(RvReg::S0, RvReg::SP, frameSize - 4);
        listing += Rv32I::addi(RvReg::SP, RvReg::SP, frameSize);
    }

private:
    int frameSize;
};
