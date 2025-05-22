#pragma once
#include "SkrInstruction.h"
#include "../value/SkrValue.h"
#include "../../frontend/ast/exp/AstBinaryExp.h"

class SkrBranch : public SkrInstruction {
public:
    enum class Operator { Equals, NotEquals };

    SkrBranch(SkrValue* left, Operator op, SkrValue* right, const char* label) 
        : SkrInstruction(Type::Branch)
        , left(left)
        , op(op)
        , right(right)
        , label(label) {  }

    SkrValue* getLeft() const { return left; }
    Operator getOperator() const { return op; }
    SkrValue* getRight() const { return right; }
    const char* getLabel() const { return label; }

private:
    SkrValue* left;
    Operator op;
    SkrValue* right;
    const char* label;
};
