#pragma once
#include "../../frontend/ast/exp/AstBinaryExp.h"
#include "../value/SkrValue.h"
#include "SkrInstruction.h"

class SkrBranch : public SkrInstruction {
public:
    enum class Operator {
        Equals,
        NotEquals,
        LessThan,
        LessOrEqual,
        GreaterThan,
        GreaterOrEqual
    };

    SkrBranch(SkrValue* left, Operator op, SkrValue* right, StringRef label)
        : SkrInstruction(Kind::Branch)
        , left(left)
        , op(op)
        , right(right)
        , label(label) { }

    SkrValue* getLeft() const { return left; }
    Operator getOperator() const { return op; }
    SkrValue* getRight() const { return right; }
    StringRef getLabel() const { return label; }

private:
    SkrValue* left;
    Operator op;
    SkrValue* right;
    StringRef label;
};
