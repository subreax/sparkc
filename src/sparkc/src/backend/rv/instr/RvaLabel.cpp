#include "sparkc/backend/rv/instr/RvaLabel.h"

RvaLabel::RvaLabel(StringRef value)
    : RvaInstruction(Kind::Label), value(value) {}

void RvaLabel::emit(RvListing& listing) { listing.addLabel(value); }

StringRef RvaLabel::getValue() const { return value; }
