#pragma once
#include "RvaInstruction.h"

class RvaJump : public RvaInstruction {
public:
    RvaJump(const char* label) : RvaInstruction(Type::Jump), label(label) {  }

    void emit(RvListing& listing) override {
        printf("Jump not implemented\n");
        std::abort();
    }

    const char* getLabel() const { return label; }

private:
    const char* label;
};