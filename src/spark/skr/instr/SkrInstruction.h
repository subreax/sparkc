#pragma once

class SkrInstruction {
public:
    enum class Type {
        Binary, Copy, Jump, Label, Branch
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
