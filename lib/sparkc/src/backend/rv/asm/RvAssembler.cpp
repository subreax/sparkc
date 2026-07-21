#include "sparkc/backend/rv/asm/RvAssembler.h"

RvAssembler::RvAssembler(uint8_t* out, size_t cap)
    : listing(out, cap) { }

void RvAssembler::addExternalLabel(StringRef label, void* ptr) {
    listing.addExternalLabel(label, ptr);
}

void RvAssembler::compile(const std::vector<RvaInstruction*>& rvas) {
    for (RvaInstruction* it : rvas) {
        it->emit(listing);
    }
}

void RvAssembler::link() {
    listing.link();
}
