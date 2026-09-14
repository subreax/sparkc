#include "sparkc/backend/rv/instr/RvaDataAlloc.h"

void RvaDataAlloc::emit(RvListing& listing) {
    listing.addGlobalVar(label, sz);
}
