#pragma once
#include <cstdint>
#include "../../lexer/Token.h"
#include "../../../common/Error.h"

class AstExp {
public:
    enum class Kind { Constant, Binary, Var, Assignment, FunCall, _Count };

    AstExp(Kind kind) : kind(kind) {  }

    static const char* kindToString(Kind kind) {
        static const char* names[] = { "const", "binary", "var", "assignment", "fun_call" };
        if (kind < Kind::_Count) {
            return names[(int) kind];
        }
        sparkError("AstExp", "Failed to convert AstExp::Kind to string");
        return "";
    }

    const Kind kind;
};
