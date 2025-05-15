#pragma once
#include "AstExp.h"

class AstBinaryExp : public AstExp {
public:
    enum class Operator {
        OP_PLUS,
        OP_MINUS,
        OP_MUL,
        OP_DIV,
        OP_REM
    };

    AstBinaryExp(AstExp* left, Operator op, AstExp* right) 
        : AstExp(EXP_BINARY) 
        , left(left)
        , op(op)
        , right(right) {  }

    AstExp* getLeft() { return left; }
    Operator getOperator() { return op; }
    AstExp* getRight() { return right; }

    static Operator toBinaryOperator(TokenKind kind) {
        switch (kind) {
        case T_PLUS:        return Operator::OP_PLUS;
        case T_HYPHEN:      return Operator::OP_MINUS;
        case T_ASTERISK:    return Operator::OP_MUL;
        case T_FWD_SLASH:   return Operator::OP_DIV;
        case T_PERCENT:     return Operator::OP_REM;
        }
        printf("Unknown binary operator: %s\n", TokenKind_toString(kind));
        std::abort();
        return Operator::OP_PLUS;
    }

    static const char* operatorToString(Operator op) {
        static const char* values[] = {
            "+", "-", "*", "/", "%"
        };
        if ((int) op < 5) {
            return values[(int) op];
        }
        return "<unknown>";
    }

private:
    AstExp* left;
    Operator op;
    AstExp* right;
};
