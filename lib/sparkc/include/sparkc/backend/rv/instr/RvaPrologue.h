#pragma once
#include "RvaInstruction.h"

class RvaPrologue : public RvaInstruction {
public:
    RvaPrologue(int32_t frameSize = 0, bool _saveRa = false);

    int32_t getFrameSize() const;
    void setFrameSize(int32_t size);

    bool willSaveRa() const;
    void saveRa();

    void emit(RvListing& listing) override;

private:
    int32_t frameSize = 0;
    bool _saveRa = false;
};
