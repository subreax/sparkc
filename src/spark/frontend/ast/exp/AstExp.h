#pragma once
#include "../../Token.h"
#include <cstdint>

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

private:
    Type type;
};
