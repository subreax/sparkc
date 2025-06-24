#pragma once
#include "AstExp.h"

class AstAssignment : public AstExp {
public:
    AstAssignment(AstExp* var, AstExp* exp) 
        : AstExp(Kind::Assignment)
        , var(var)
        , exp(exp) {  }

    AstExp* getVar() { return var; }
    AstExp* getExp() { return exp; }
    void setExp(AstExp* exp) { this->exp = exp; }

private:
    AstExp* var;
    AstExp* exp;
};