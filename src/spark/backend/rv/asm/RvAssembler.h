#pragma once
#include <vector>
#include "RvListing.h"
#include "../instr/everything.h"

class RvAssembler {
public:
    static size_t assemble(const std::vector<RvaInstruction*>& instructions, uint8_t* out, size_t cap) {
        RvListing listing(out, cap);
        for (RvaInstruction* it : instructions) {
            it->emit(listing);
        }
        listing.link();
        return listing.getSize();
    }
};
