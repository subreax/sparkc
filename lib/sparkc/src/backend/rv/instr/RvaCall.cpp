#include "sparkc/backend/rv/instr/RvaCall.h"
#include "../asm/Rv32I.h"

RvaCall::RvaCall(StringRef funName)
    : RvaInstruction(Kind::Call)
    , funName(funName) { }

void RvaCall::emit(RvListing& listing) {
    listing.addWithLabel(Rv32I::auipc(RvReg::RA, 0), funName);
    listing.add(Rv32I::jalr(RvReg::RA, RvReg::RA, 0));
}

StringRef RvaCall::getFunName() const { return funName; }
