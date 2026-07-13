#pragma once
#include "sparkc/backend/rv/asm/RvListing.h"
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

    const std::vector<RvListing::Label>& getPublicLabels() const;

    size_t getSize() const;

private:
    RvListing listing;
    std::vector<RvListing::Label> publicLabels;
};
