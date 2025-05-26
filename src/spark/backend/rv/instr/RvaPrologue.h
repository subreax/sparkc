#pragma once
#include "RvaInstruction.h"
#include "../asm/Rv32I.h"

class RvaPrologue : public RvaInstruction {
public:
    RvaPrologue(int32_t frameSize = 0, bool _saveRa = false) 
        : RvaInstruction(Kind::Prologue)
        , frameSize(frameSize)
        , _saveRa(_saveRa) {  }

    int32_t getFrameSize() const { return frameSize; }
    void setFrameSize(int32_t size) { frameSize = size; }

    bool willSaveRa() const { return _saveRa; }
    void saveRa() { _saveRa = true; }

    void emit(RvListing& listing) override {
        if (frameSize == 0) return;

        listing += Rv32I::addi(RvReg::SP, RvReg::SP, -frameSize);
        if (_saveRa) {
            listing += Rv32I::sw(RvReg::SP, frameSize - 8, RvReg::RA);
        }
        listing += Rv32I::sw(RvReg::SP, frameSize - 4, RvReg::S0);
        listing += Rv32I::addi(RvReg::S0, RvReg::SP, frameSize);
    }

private:
    int32_t frameSize = 0;
    bool _saveRa = false;
};
