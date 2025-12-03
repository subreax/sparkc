#pragma once
#include "AstExp.h"

class AstVar : public AstExp {
public:
    AstVar(StringRef id) : AstExp(Kind::Var), id(id) {  }

    StringRef getId() const { return id; }
    void setId(StringRef id) { this-> id = id; }

private:
    StringRef id;
};