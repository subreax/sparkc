#pragma once
#include "../value/SkrValue.h"

class SkrInstruction {
public:
    enum class Kind {
        Binary, Copy, Jump, Label, Branch, FunCall, Int2Float, Float2Int, Load, Store, GetAddr
    };

    SkrInstruction(Kind kind) : kind(kind) {  }
    SkrInstruction(const SkrInstruction&) = delete;
    SkrInstruction& operator=(const SkrInstruction&) = delete;
    SkrInstruction(SkrInstruction&&) = delete;

    const Kind kind;
};
