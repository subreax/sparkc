#pragma once
#include <vector>
#include "AstExp.h"
#include "../../../common/BoundArray.h"

class AstFunCall : public AstExp {
public:
    AstFunCall(const char* funName, const BoundArray<AstExp*>& args)
        : AstExp(Kind::FunCall)
        , funName(funName)
        , args(args) {  }

    const char* getFunName() const { return funName; }
    BoundArray<AstExp*>& getArgs() { return args; }

private:
    const char* funName;
    BoundArray<AstExp*> args;
};