#pragma once

class SkrInstruction {
public:
    enum class Type {
        Binary
    };

    SkrInstruction(Type type) : type(type) {  }
    SkrInstruction(const SkrInstruction&) = delete;
    SkrInstruction& operator=(const SkrInstruction&) = delete;
    SkrInstruction(SkrInstruction&&) = delete;

    Type getType() const { return type; }

private:
    Type type;  
};
