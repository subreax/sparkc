#pragma once
#include "../value/SkrValue.h"
#include "SkrInstruction.h"

class SkrBinary : public SkrInstruction {
public:
    enum class Operator {
        Plus,
        Minus,
        Mul,
        Div,
        Rem,
        Equals,
        NotEquals,
        LessThan,
        LessOrEqual,
        GreaterThan,
        GreaterOrEqual
    };

    SkrBinary(SkrVar* dst, SkrValue* left, Operator op, SkrValue* right)
        : SkrInstruction(Kind::Binary)
        , dst(dst)
        , left(left)
        , op(op)
        , right(right) { }

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
