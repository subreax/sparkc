#pragma once
#include "sparkc/symbol/SymbolType.h"
#include "sparkc/frontend/lexer/Token.h"
#include "sparkc/common/Error.h"
#include <cstdint>

#define SPARK_AST_EXP_KINDS_LIST(X) \
    X(Constant, "constant")         \
    X(Binary, "binary")             \
    X(Var, "var")                   \
    X(Assignment, "assignment")     \
    X(FunCall, "fun call")          \
    X(Cast, "cast")                 \
    X(Dereference, "dereference")   \
    X(AddrOf, "addr of")            \
    X(Dot, "dot")                   \
    X(StructInit, "struct init")

class AstExp {
public:
    enum class Kind {
#define X(kind, name) kind,
        SPARK_AST_EXP_KINDS_LIST(X)
#undef X
            _Count
    };

    AstExp(Kind kind, SymbolType* type = nullptr)
        : kind(kind)
        , type(type) { }

    static const char* kindToString(Kind kind);

    bool hasType(const SymbolType* other) const {
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
