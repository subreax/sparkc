#pragma once
#include "AstExp.h"

class AstBinaryExp : public AstExp {
public:
    enum class Operator {
        Plus,
        Minus,
        Mul,
        Div,
        Rem,
        And,
        Or
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
        case T_AMP_AMP:     return Operator::And;
        case T_VBAR_VBAR:   return Operator::Or;
        }
        printf("Unknown binary operator: %s\n", TokenKind_toString(kind));
        std::abort();
        return Operator::Plus;
    }

    static const char* operatorToString(Operator op) {
        static const char* values[] = {
            "+", "-", "*", "/", "%", "and", "or"
        };
        auto valuesCount = sizeof(values) / sizeof(const char*);
        if ((int) op < valuesCount) {
            return values[(int) op];
        }
        return "<unknown>";
    }

private:
    AstExp* left;
    Operator op;
    AstExp* right;
};
