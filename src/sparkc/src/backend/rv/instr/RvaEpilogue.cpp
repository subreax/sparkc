#include "sparkc/backend/rv/instr/RvaEpilogue.h"
#include "../asm/Rv32I.h"

RvaEpilogue::RvaEpilogue(int32_t frameSize, bool _loadRa)
    : RvaInstruction(Kind::Epilogue), frameSize(frameSize), _loadRa(_loadRa) {}

int32_t RvaEpilogue::getFrameSize() const { return frameSize; }
void RvaEpilogue::setFrameSize(int32_t size) { frameSize = size; }

bool RvaEpilogue::willLoadRa() const { return _loadRa; }
void RvaEpilogue::loadRa() { _loadRa = true; }

void RvaEpilogue::emit(RvListing& listing) {
    if (frameSize == 0)
        return;

    if (_loadRa) {
        listing += Rv32I::lw(RvReg::RA, RvReg::SP, frameSize - 8);
    }
    listing += Rv32I::lw(RvReg::S0, RvReg::SP, frameSize - 4);
    listing += Rv32I::addi(RvReg::SP, RvReg::SP, frameSize);
}
