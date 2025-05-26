#pragma once
#include <cstdint>
#include "../../lexer/Token.h"

class AstExp {
public:
    enum Type {
        EXP_CONSTANT,
        EXP_BINARY,
        EXP_VAR,
        EXP_ASSIGNMENT,
        EXP_FUN_CALL
    };

    AstExp(Type type) : type(type) {  }
    Type getType() const { return type; }

    static const char* typeToString(Type type) {
        switch (type) {
        case EXP_CONSTANT: return "const";
        case EXP_BINARY: return "binary";
        case EXP_VAR: return "var";
        case EXP_ASSIGNMENT: return "assignment";
        case EXP_FUN_CALL: return "fun_call";
        default: return "<unknown type>";
        }
    }

private:
    Type type;
};
