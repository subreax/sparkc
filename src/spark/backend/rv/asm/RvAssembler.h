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

    void addExternalLabel(StringRef label, void* ptr) {
        listing.addExternalLabel(label, ptr);
    }

    void compile(const std::vector<RvaInstruction*>& rvas) {
        for (RvaInstruction* it : rvas) {
            it->emit(listing);
        }
    }

    void link() {
        listing.link();
        listing.getPublicLabels(publicLabels);
    }

    const std::vector<RvListing::Label>& getPublicLabels() const { 
        return publicLabels;
    }

    size_t getSize() const { return listing.getSize(); }

private:
    RvListing listing;
    std::vector<RvListing::Label> publicLabels;
};
