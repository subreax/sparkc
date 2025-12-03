#include "sparkc/backend/rv/instr/RvaLoad.h"
#include "../asm/Rv32I.h"

RvaLoad::RvaLoad(RvaValue* to, RvaMemory* from)
    : RvaInstruction(Kind::Load), to(to), from(from) {}

void RvaLoad::emit(RvListing& listing) {
    listing += Rv32I::lw(expectReg(to), from->getBase(), from->getOffset());
}
