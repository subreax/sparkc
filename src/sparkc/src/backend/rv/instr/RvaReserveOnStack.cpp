#include "sparkc/backend/rv/instr/RvaReserveOnStack.h"
#include "sparkc/common/Error.h"

RvaReserveOnStack::RvaReserveOnStack(RvaValue* mem)
    : RvaInstruction(Kind::ReserveOnStack)
    , mem(mem) { }

void RvaReserveOnStack::emit(RvListing& listing) {
    sparkError("RvaReserveOnStack", "emit() shouldn't be called");
}
