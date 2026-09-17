#include "sparkc/backend/rv/instr/RvaGetAddress.h"
#include "../asm/Rv32I.h"

RvaGetAddress::RvaGetAddress(RvaValue* to, RvaValue* of)
    : RvaInstruction(Kind::GetAddress)
    , to(to)
    , of(of) { }

void RvaGetAddress::emit(RvListing& listing) {
    if (of->kind == RvaValue::Kind::Data) {
        auto* ofData = (RvaData*) of;
        auto toReg = expectReg(to);
        listing.addWithLabel(Rv32I::auipc(toReg, 0), ofData->getLabel());
        listing += Rv32I::addi(toReg, toReg, ofData->getOffset());
    }
    else {
        sparkError("RvaGetAddress", "I can only handle RvaData operand");
    }
}
