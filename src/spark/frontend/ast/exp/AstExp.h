#pragma once
#include <cstdint>
#include "../../lexer/Token.h"
#include "../../../common/Error.h"
#include "../../../symbol/SymbolType.h"

class AstExp {
public:
    enum class Kind { Constant, Binary, Var, Assignment, FunCall, Cast, Dereference, AddrOf, Dot, _Count };

    AstExp(Kind kind, SymbolType* type = nullptr)
        : kind(kind)
        , type(type) {  }

    static const char* kindToString(Kind kind) {
        static const char* names[] = { "const", "binary", "var", "assignment", "fun call", "cast", "dereference", "get addr", "dot" };
        if (kind < Kind::_Count) {
            return names[(int) kind];
        }
        sparkError("AstExp", "Failed to convert AstExp::Kind to string");
        return "";
    }

    bool hasType(SymbolType* other) const {
        if (type == nullptr) {
            return false;
        }

        return type->kind == other->kind;
    }

    bool hasType(SymbolType::Kind other) const {
        if (type == nullptr) {
            return false;
        }

        return type->kind == other;
    }

    const Kind kind;
    SymbolType* type = nullptr;
};
