#pragma once
#include "AstExp.h"

class AstDot : public AstExp {
public:
    AstDot(AstExp* from, AstExp* field) : AstExp(Kind::Dot), from(from), field(field) {  }

    AstExp* getFrom() const { return from; }
    void setFrom(AstExp* exp) { from = exp; }

    AstExp* getField() const { return field; }

private:
    AstExp* from;
    AstExp* field;
};
