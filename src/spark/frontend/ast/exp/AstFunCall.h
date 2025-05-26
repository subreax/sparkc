#pragma once
#include <vector>
#include "AstExp.h"
#include "../../../common/BoundArray.h"

class AstFunCall : public AstExp {
public:
    AstFunCall(const char* funName, const BoundArray<AstExp*>& args)
        : AstExp(EXP_FUN_CALL)
        , funName(funName)
        , args(args) {  }

    const char* getFunName() const { return funName; }
    const BoundArray<AstExp*>& getArgs() const { return args; }

private:
    const char* funName;
    BoundArray<AstExp*> args;
};