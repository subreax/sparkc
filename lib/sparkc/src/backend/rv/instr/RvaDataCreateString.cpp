#include "sparkc/backend/rv/instr/RvaDataCreateString.h"

void RvaDataCreateString::emit(RvListing& listing) {
    listing.addStringConst(label, str);
}
