#pragma once
#include "SkrInstruction.h"

class SkrLabel : public SkrInstruction {
public:
    SkrLabel(const char* label) : SkrInstruction(Kind::Label), label(label) {  }

    const char* getLabel() const { return label; }

private:
    const char* label;
};