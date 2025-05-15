#pragma once
#include "../../Token.h"
#include <cstdint>

class AstExp {
public:
    enum Type {
        EXP_CONSTANT,
        EXP_BINARY
    };

    AstExp(Type type) : type(type) {  }
    Type getType() const { return type; }

private:
    Type type;
};
