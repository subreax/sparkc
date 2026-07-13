#include "sparkc/backend/rv/instr/RvaStore.h"
#include "../asm/Rv32I.h"

RvaStore::RvaStore(RvaMemory* to, RvaValue* from)
    : RvaInstruction(Kind::Store)
    , to(to)
    , from(from) { }

void RvaStore::emit(RvListing& listing) {
    listing += Rv32I::sw(to->getBase(), to->getOffset(), expectReg(from));
}
