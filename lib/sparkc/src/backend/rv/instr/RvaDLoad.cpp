#include "sparkc/backend/rv/instr/RvaDLoad.h"
#include "../asm/Rv32I.h"

void RvaDLoad::emit(RvListing& listing) {
    listing.addWithLabel(Rv32I::auipc(tempOffsetReg->getReg(), 0), src->getLabel());
    listing.add(Rv32I::lw(dst->getReg(), tempOffsetReg->getReg(), 0));
}
