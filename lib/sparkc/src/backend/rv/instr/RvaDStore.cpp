#include "sparkc/backend/rv/instr/RvaDStore.h"
#include "../asm/Rv32I.h"

void RvaDStore::emit(RvListing& listing) {
    listing.addWithLabel(Rv32I::auipc(tempOffsetReg->getReg(), 0), dst->getLabel());
    listing.add(Rv32I::sw(tempOffsetReg->getReg(), 0, src->getReg()));
}
