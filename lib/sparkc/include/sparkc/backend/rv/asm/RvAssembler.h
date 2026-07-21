#pragma once
#include "sparkc/backend/rv/asm/RvListing.h"
#include "sparkc/backend/rv/asm/Label.h"
#include "sparkc/backend/rv/instr/RvaInstruction.h"
#include <vector>

class RvAssembler {
public:
    RvAssembler(uint8_t* out, size_t cap);

    RvAssembler(const RvAssembler&) = delete;
    RvAssembler& operator=(const RvAssembler&) = delete;

    void addExternalLabel(StringRef label, void* ptr);
    void compile(const std::vector<RvaInstruction*>& rvas);
    void link();

    std::vector<Label> getPublicLabels() const {
        return listing.getPublicLabels();
    }

    size_t getSize() const {
        return listing.getSize();
    }

private:
    RvListing listing;
};
