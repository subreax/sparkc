#pragma once
#include "SkrInstruction.h"

class SkrLabel : public SkrInstruction {
public:
    SkrLabel(StringRef label)
        : SkrInstruction(Kind::Label)
        , label(label) { }

    StringRef getLabel() const { return label; }

private:
    StringRef label;
};