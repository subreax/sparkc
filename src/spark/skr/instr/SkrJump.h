#pragma once
#include "SkrInstruction.h"

class SkrJump : public SkrInstruction {
public:
    SkrJump(const char* label) : SkrInstruction(Type::Jump), label(label) {  }

    const char* getLabel() { return label; }

private:
    const char* label;
};