#pragma once
#include "RvaInstruction.h"

class RvaEpilogue : public RvaInstruction {
public:
    RvaEpilogue(int frameSize) : RvaInstruction(Type::Epilogue), frameSize(frameSize) {  }

    int getFrameSize() const { return frameSize; }
    void setFrameSize(int size) { frameSize = size; }

    void emit(RvListing& listing) override {}

private:
    int frameSize;
};
