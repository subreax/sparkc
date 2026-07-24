#include "sparkc/frontend/ast/exp/AstBinaryExp.h"

static const char* OPERATOR_STRING_VALUES[] = {
#define X(kind, name) name,
    SPARK_AST_BINARY_EXP_OPERATORS_LIST(X)
#undef X
};

static constexpr size_t OPERATORS_COUNT = (size_t) AstBinaryExp::Operator::_Count;

AstBinaryExp::Operator AstBinaryExp::toBinaryOperator(TokenKind kind) {
    switch (kind) {
    case T_PLUS: return Operator::Plus;
    case T_HYPHEN: return Operator::Minus;
    case T_ASTERISK: return Operator::Mul;
    case T_FWD_SLASH: return Operator::Div;
    case T_PERCENT: return Operator::Rem;
    case T_AMP_AMP: return Operator::And;
    case T_VBAR_VBAR: return Operator::Or;
    case T_EQUALS_EQUALS: return Operator::Equals;
    case T_NOT_EQUALS: return Operator::NotEquals;
    case T_LESS_THAN: return Operator::LessThan;
    case T_LESS_OR_EQ: return Operator::LessOrEqual;
    case T_GREATER_THAN: return Operator::GreaterThan;
    case T_GREATER_OR_EQ: return Operator::GreaterOrEqual;
    default:
        sparkError("AstBinaryExp", "Unknown operator: %s", TokenKind_toString(kind));
    }
    return Operator::Plus;
}

const char* AstBinaryExp::operatorToString(Operator op) {
    size_t op1 = (size_t) op;
    if (op1 < OPERATORS_COUNT) {
        return OPERATOR_STRING_VALUES[op1];
    }
    sparkError("AstExp", "Unknown AstBinaryExp::Operator %d", op1);
    return "";
}