#include "sparkc/backend/rv/instr/RvaBeginTempStack.h"
#include "sparkc/common/Error.h"

RvaBeginTempStack::RvaBeginTempStack()
    : RvaInstruction(Kind::BeginTempStack) { }

void RvaBeginTempStack::emit(RvListing& listing) {
    sparkError("RvaBeginTempStack", "emit() shouldn't be called");
}
