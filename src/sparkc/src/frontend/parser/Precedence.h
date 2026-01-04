#pragma once
#include "sparkc/frontend/lexer/Token.h"

class Precedence {
public:
    static const int NOT_SUPPORTED = -1;

    static int get(TokenKind kind) {
        switch (kind) {
        case T_PERIOD:
            return 100;

        case T_ASTERISK:
        case T_FWD_SLASH:
        case T_PERCENT:
            return 50;

        case T_PLUS:
        case T_HYPHEN:
            return 40;

        case T_GREATER_THAN:
        case T_LESS_THAN:
        case T_GREATER_OR_EQ:
        case T_LESS_OR_EQ:
            return 35;

        case T_EQUALS_EQUALS:
        case T_NOT_EQUALS:
            return 30;

        case T_AMP_AMP:
            return 10;

        case T_VBAR_VBAR:
            return 5;

        case T_EQUALS:
            return 1;

        default:
            return NOT_SUPPORTED;
        }
    }
};