#pragma once
#include "SkrInstruction.h"
#include "../value/SkrValue.h"


class SkrBinary : public SkrInstruction {
public:
    enum class Operator { Plus, Minus, Mul, Div, Rem, And, Or };

    SkrBinary(SkrVar* dst, SkrValue* left, Operator op, SkrValue* right)
        : SkrInstruction(Type::Binary)
        , dst(dst)
        , left(left)
        , op(op)
        , right(right) {  }

    SkrVar* getDst() { return dst; }
    SkrValue* getLeft() { return left; }
    Operator getOperator() { return op; }
    SkrValue* getRight() { return right; }

private:
    SkrVar* dst;
    SkrValue* left;
    Operator op;
    SkrValue* right;
};
