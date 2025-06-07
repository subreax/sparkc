#pragma once
#include "AstExp.h"
#include "../../../common/Constant.h"

class AstConstantExp : public AstExp {
public:
    AstConstantExp(Constant* val) : AstExp(Kind::Constant, val->type), val(val) {  }

    Constant* getValue() const { return val; }

private:
    Constant* val;
};
