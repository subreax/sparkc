#pragma once
#include "AstExp.h"

class AstAssignment : public AstExp {
public:
    AstAssignment(AstExp* var, AstExp* exp) 
        : AstExp(EXP_ASSIGNMENT)
        , var(var)
        , exp(exp) {  }

    AstExp* getVar() { return var; }
    AstExp* getExp() { return exp; }

private:
    AstExp* var;
    AstExp* exp;
};