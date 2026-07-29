#pragma once
#include "../value/SkrValue.h"
#include "SkrInstruction.h"

#define SKR_BINARY_OP_LIST(X) \
    X(Plus, "+")              \
    X(Minus, "-")             \
    X(Mul, "*")               \
    X(Div, "/")               \
    X(Rem, "%")               \
    X(Equals, "=")            \
    X(NotEquals, "!=")        \
    X(LessThan, "<")          \
    X(LessOrEqual, "<=")      \
    X(GreaterThan, ">")       \
    X(GreaterOrEqual, ">=")

class SkrBinary : public SkrInstruction {
public:
    enum class Operator {
#define X(id, name) id,
        SKR_BINARY_OP_LIST(X)
            _Count
#undef X
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

    static const char* operatorStringValue(Operator op);

private:
    SkrVar* dst;
    SkrValue* left;
    Operator op;
    SkrValue* right;
};
