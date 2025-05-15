#pragma once
#include "AstExp.h"

class AstConstantExp : public AstExp {
public:
    AstConstantExp(int32_t value) : AstExp(EXP_CONSTANT), value(value) {  }

    int32_t getValue() const { return value; }

private:
    int32_t value;
};
