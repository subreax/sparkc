#include "sparkc/backend/rv/instr/RvaJump.h"
#include "../asm/Rv32I.h"

RvaJump::RvaJump(StringRef label) : RvaInstruction(Kind::Jump), label(label) {}

void RvaJump::emit(RvListing& listing) {
    listing.addWithLabel(Rv32I::jal(RvReg::ZERO), label);
}

StringRef RvaJump::getLabel() const { return label; }
