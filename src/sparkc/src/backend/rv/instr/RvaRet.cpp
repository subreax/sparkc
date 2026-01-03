#include "sparkc/backend/rv/instr/RvaRet.h"
#include "../asm/Rv32I.h"

RvaRet::RvaRet()
    : RvaInstruction(Kind::Ret) { }

void RvaRet::emit(RvListing& listing) {
    listing += Rv32I::ret();
}
