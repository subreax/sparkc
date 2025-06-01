#pragma once
#include "AstExp.h"
#include "../../../common/Error.h"

class AstBinaryExp : public AstExp {
public:
    enum class Operator {
        Plus,
        Minus,
        Mul,
        Div,
        Rem,
        And,
        Or,
        Equals,
        NotEquals,
        LessThan,
        LessOrEqual,
        GreaterThan,
        GreaterOrEqual
    };

    AstBinaryExp(AstExp* left, Operator op, AstExp* right, SymbolType* type = nullptr)
        : AstExp(Kind::Binary, type) 
        , left(left)
        , op(op)
        , right(right) {  }

    AstExp* getLeft() { return left; }
    void setLeft(AstExp* exp) { left = exp; }

    Operator getOperator() { return op; }

    AstExp* getRight() { return right; }
    void setRight(AstExp* exp) { right = exp; }

    static Operator toBinaryOperator(TokenKind kind) {
        switch (kind) {
        case T_PLUS:        return Operator::Plus;
        case T_HYPHEN:      return Operator::Minus;
        case T_ASTERISK:    return Operator::Mul;
        case T_FWD_SLASH:   return Operator::Div;
        case T_PERCENT:     return Operator::Rem;
        case T_AMP_AMP:     return Operator::And;
        case T_VBAR_VBAR:   return Operator::Or;
        case T_EQUALS_EQUALS: return Operator::Equals;
        case T_NOT_EQUALS:  return Operator::NotEquals;
        case T_LESS_THAN:   return Operator::LessThan;
        case T_LESS_OR_EQ:  return Operator::LessOrEqual;
        case T_GREATER_THAN: return Operator::GreaterThan;
        case T_GREATER_OR_EQ: return Operator::GreaterOrEqual;
        default: 
            sparkError("AstBinaryExp", "Unknown operator: %s", TokenKind_toString(kind));
        }
        return Operator::Plus;
    }

    static const char* operatorToString(Operator op) {
        static const char* values[] = {
            "+", "-", "*", "/", "%", "and", "or", "==", "!=", "<", "<=", ">", ">="
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
