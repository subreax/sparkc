#include "sparkc/backend/rv/instr/RvaEndTempStack.h"

RvaEndTempStack::RvaEndTempStack() : RvaInstruction(Kind::EndTempStack) {}

void RvaEndTempStack::emit(RvListing& listing) {
    sparkError("RvaEndTempStack", "emit() shouldn't be called");
}
