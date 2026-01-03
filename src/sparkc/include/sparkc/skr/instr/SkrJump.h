#pragma once
#include "SkrInstruction.h"

class SkrJump : public SkrInstruction {
public:
    SkrJump(StringRef label)
        : SkrInstruction(Kind::Jump)
        , label(label) { }

    StringRef getLabel() { return label; }

private:
    StringRef label;
};