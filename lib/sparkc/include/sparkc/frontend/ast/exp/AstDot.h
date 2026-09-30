#pragma once
#include "AstExp.h"

class AstDot : public AstExp {
public:
    AstDot(AstExp* from, int depth, AstExp* field)
        : AstExp(Kind::Dot)
        , from(from)
        , depth(depth)
        , field(field) { }

    AstExp* getFrom() const { return from; }
    void setFrom(AstExp* exp) { from = exp; }

    int getDepth() const { return depth; }
    void setDepth(int depth) { this->depth = depth; }

    AstExp* getField() const { return field; }

private:
    AstExp* from;
    int depth;
    AstExp* field;
};
