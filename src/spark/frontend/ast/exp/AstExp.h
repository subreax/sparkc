#pragma once
#include <cstdint>
#include "../../lexer/Token.h"
#include "../../../common/Error.h"
#include "../../../symbol/SymbolType.h"

class AstExp {
public:
    enum class Kind { Constant, Binary, Var, Assignment, FunCall, Cast, _Count };

    AstExp(Kind kind, SymbolType* type = nullptr)
        : kind(kind)
        , type(type) {  }

    static const char* kindToString(Kind kind) {
        static const char* names[] = { "const", "binary", "var", "assignment", "fun_call", "cast" };
        if (kind < Kind::_Count) {
            return names[(int) kind];
        }
        sparkError("AstExp", "Failed to convert AstExp::Kind to string");
        return "";
    }

    const Kind kind;
    SymbolType* type = nullptr;
};
