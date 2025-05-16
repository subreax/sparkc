#pragma once
#include "AstExp.h"

class AstBinaryExp : public AstExp {
public:
    enum class Operator {
        Plus,
        Minus,
        Mul,
        Div,
        Rem
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
        case T_PLUS:        return Operator::Plus;
        case T_HYPHEN:      return Operator::Minus;
        case T_ASTERISK:    return Operator::Mul;
        case T_FWD_SLASH:   return Operator::Div;
        case T_PERCENT:     return Operator::Rem;
        }
        printf("Unknown binary operator: %s\n", TokenKind_toString(kind));
        std::abort();
        return Operator::Plus;
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
