#pragma once

class SkrInstruction {
public:
    enum class Type {
        Binary
    };

    SkrInstruction(Type type) : type(type) {  }

    Type getType() const { return type; }

private:
    Type type;  
};
