#include "sparkc/backend/rv/instr/RvaPrologue.h"
#include "../asm/Rv32I.h"

RvaPrologue::RvaPrologue(int32_t frameSize, bool _saveRa)
    : RvaInstruction(Kind::Prologue)
    , frameSize(frameSize)
    , _saveRa(_saveRa) { }

int32_t RvaPrologue::getFrameSize() const { return frameSize; }
void RvaPrologue::setFrameSize(int32_t size) { frameSize = size; }

bool RvaPrologue::willSaveRa() const { return _saveRa; }
void RvaPrologue::saveRa() { _saveRa = true; }

void RvaPrologue::emit(RvListing& listing) {
    if (frameSize == 0)
        return;

    listing += Rv32I::addi(RvReg::SP, RvReg::SP, -frameSize);
    if (_saveRa) {
        listing += Rv32I::sw(RvReg::SP, frameSize - 8, RvReg::RA);
    }
    listing += Rv32I::sw(RvReg::SP, frameSize - 4, RvReg::S0);
    listing += Rv32I::addi(RvReg::S0, RvReg::SP, frameSize);
}
