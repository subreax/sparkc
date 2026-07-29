#pragma once
#include "../../frontend/ast/exp/AstBinaryExp.h"
#include "../value/SkrValue.h"
#include "SkrInstruction.h"

#define SKR_BRANCH_OP_LIST(X) \
    X(Equals, "==")           \
    X(NotEquals, "!=")        \
    X(LessThan, "<")          \
    X(LessOrEqual, "<=")      \
    X(GreaterThan, ">")       \
    X(GreaterOrEqual, ">=")

class SkrBranch : public SkrInstruction {
public:
    enum class Operator {
#define X(id, name) id,
        SKR_BRANCH_OP_LIST(X)
#undef X
            _Count
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

    static const char* operatorStringValue(Operator op);

private:
    SkrValue* left;
    Operator op;
    SkrValue* right;
    StringRef label;
};
