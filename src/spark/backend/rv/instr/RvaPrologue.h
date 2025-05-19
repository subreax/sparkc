#pragma once
#include "RvaInstruction.h"

class RvaPrologue : public RvaInstruction {
public:
    RvaPrologue(int frameSize) : RvaInstruction(Type::Prologue), frameSize(frameSize) {  }

    int getFrameSize() const { return frameSize; }
    void setFrameSize(int size) { frameSize = size; }

    void emit(RvListing& listing) override {}

private:
    int frameSize;
};
