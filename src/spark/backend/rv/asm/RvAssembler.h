#pragma once
#include <vector>
#include "RvListing.h"
#include "../instr/everything.h"

class RvAssembler {
public:
    static size_t assemble(const std::vector<RvaInstruction*>& instructions, uint32_t* out, size_t cap) {
        std::vector<uint32_t> bin;
        RvListing listing(bin);
        for (RvaInstruction* it : instructions) {
            it->emit(listing);
        }

        if (bin.size() > cap / 4) {
            return 0;
        }

        for (uint32_t i = 0; i < bin.size(); i++) {
            out[i] = bin[i];
        }
        return bin.size() * 4;
    }
};
