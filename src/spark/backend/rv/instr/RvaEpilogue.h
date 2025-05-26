#pragma once
#include "RvaInstruction.h"

class RvaEpilogue : public RvaInstruction {
public:
    RvaEpilogue(int32_t frameSize = 0, bool _loadRa = false) 
        : RvaInstruction(Type::Epilogue)
        , frameSize(frameSize)
        , _loadRa(_loadRa) {  }

    int32_t getFrameSize() const { return frameSize; }
    void setFrameSize(int32_t size) { frameSize = size; }

    bool willLoadRa() const { return _loadRa; }
    void loadRa() { _loadRa = true; }

    void emit(RvListing& listing) override {
        if (frameSize == 0) return;

        if (_loadRa) {
            listing += Rv32I::lw(RvReg::RA, RvReg::SP, frameSize - 8);
        }
        listing += Rv32I::lw(RvReg::S0, RvReg::SP, frameSize - 4);
        listing += Rv32I::addi(RvReg::SP, RvReg::SP, frameSize);
    }

private:
    int32_t frameSize = 0;
    bool _loadRa = false;
};
