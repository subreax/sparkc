#pragma once
#include "AstExp.h"
#include "sparkc/common/Error.h"

#define SPARK_AST_BINARY_EXP_OPERATORS_LIST(X) \
    X(Plus, "+")                               \
    X(Minus, "-")                              \
    X(Mul, "*")                                \
    X(Div, "/")                                \
    X(Rem, "%")                                \
    X(And, "and")                              \
    X(Or, "or")                                \
    X(Equals, "==")                            \
    X(NotEquals, "!=")                         \
    X(LessThan, "<")                           \
    X(LessOrEqual, "<=")                       \
    X(GreaterThan, ">")                        \
    X(GreaterOrEqual, ">=")

class AstBinaryExp : public AstExp {
public:
    enum class Operator {
#define X(name, string) name,
        SPARK_AST_BINARY_EXP_OPERATORS_LIST(X)
#undef X
            _Count
    };

    AstBinaryExp(AstExp* left, Operator op, AstExp* right, SymbolType* type = nullptr)
        : AstExp(Kind::Binary, type)
        , left(left)
        , op(op)
        , right(right) { }

    AstExp* getLeft() { return left; }
    void setLeft(AstExp* exp) { left = exp; }

    Operator getOperator() { return op; }

    AstExp* getRight() { return right; }
    void setRight(AstExp* exp) { right = exp; }

    static Operator toBinaryOperator(TokenKind kind);

    static const char* operatorToString(Operator op);

private:
    AstExp* left;
    Operator op;
    AstExp* right;
};
