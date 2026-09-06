#include "sparkc/backend/rv/instr/RvaCall.h"
#include "../asm/Rv32I.h"

RvaCall::RvaCall(StringRef funName, RvReg offsetReg)
    : RvaInstruction(Kind::Call)
    , funName(funName)
    , offsetReg(offsetReg) { }

void RvaCall::emit(RvListing& listing) {
    listing.addWithLabel(Rv32I::auipc(offsetReg, 0), funName);
    listing.add(Rv32I::jalr(RvReg::RA, offsetReg, 0));
}

StringRef RvaCall::getFunName() const { return funName; }

RvReg RvaCall::getOffsetReg() const { return offsetReg; }