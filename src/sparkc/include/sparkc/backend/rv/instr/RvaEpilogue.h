#pragma once
#include "RvaInstruction.h"

class RvaEpilogue : public RvaInstruction {
public:
    RvaEpilogue(int32_t frameSize = 0, bool _loadRa = false);

    int32_t getFrameSize() const;
    void setFrameSize(int32_t size);

    bool willLoadRa() const;
    void loadRa();

    void emit(RvListing& listing) override;

private:
    int32_t frameSize = 0;
    bool _loadRa = false;
};
