#include "sparkc/backend/rv/asm/RvAssembler.h"

RvAssembler::RvAssembler(MemBlockRef outBin)
    : listing(outBin) { }

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
