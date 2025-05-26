#pragma once

class SkrInstruction {
public:
    enum class Kind {
        Binary, Copy, Jump, Label, Branch, FunCall
    };

    SkrInstruction(Kind kind) : kind(kind) {  }
    SkrInstruction(const SkrInstruction&) = delete;
    SkrInstruction& operator=(const SkrInstruction&) = delete;
    SkrInstruction(SkrInstruction&&) = delete;

    const Kind kind;
};
