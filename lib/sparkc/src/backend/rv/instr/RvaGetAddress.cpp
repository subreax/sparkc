#include "sparkc/backend/rv/instr/RvaGetAddress.h"

RvaGetAddress::RvaGetAddress(RvaValue* to, RvaValue* of)
    : RvaInstruction(Kind::GetAddress)
    , to(to)
    , of(of) { }

void RvaGetAddress::emit(RvListing& listing) {
    sparkError("RvaGetAddress", "emit shouldn't be called");
}
