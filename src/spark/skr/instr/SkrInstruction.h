#pragma once

class SkrInstruction {
public:
    enum class Type {
        Binary, Copy, Jump, Label, Branch, FunCall
    };

    SkrInstruction(Type type) : type(type) {  }
    SkrInstruction(const SkrInstruction&) = delete;
    SkrInstruction& operator=(const SkrInstruction&) = delete;
    SkrInstruction(SkrInstruction&&) = delete;
    virtual ~SkrInstruction() = default;

    Type getType() const { return type; }

private:
    Type type;  
};
