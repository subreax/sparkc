#pragma once
#include <vector>
#include "RvListing.h"
#include "../instr/everything.h"

class RvAssembler {
public:
    RvAssembler(uint8_t* out, size_t cap)
        : listing(out, cap) {  }

    RvAssembler(const RvAssembler&) = delete;
    RvAssembler& operator=(const RvAssembler&) = delete;

    void compile(const std::vector<RvaInstruction*>& rvas) {
        for (RvaInstruction* it : rvas) {
            it->emit(listing);
        }
    }

    void link() {
        listing.link();
        listing.getExternalLabels(externalLabels);
    }

    const std::vector<RvListing::Label>& getExternalLabels() const { 
        return externalLabels;
    }

    size_t getSize() const { return listing.getSize(); }

private:
    RvListing listing;
    std::vector<RvListing::Label> externalLabels;
};
