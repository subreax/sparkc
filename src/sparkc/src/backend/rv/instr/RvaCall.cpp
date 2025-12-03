#include "sparkc/backend/rv/instr/RvaCall.h"
#include "../asm/Rv32I.h"

RvaCall::RvaCall(StringRef funName)
    : RvaInstruction(Kind::Call), funName(funName) {}

void RvaCall::emit(RvListing& listing) {
    listing.addWithLabel(Rv32I::jal(RvReg::RA), funName);
}

StringRef RvaCall::getFunName() const { return funName; }
