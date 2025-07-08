#pragma once
#include <vector>
#include "AstExp.h"
#include "../../../common/BoundArray.h"

class AstFunCall : public AstExp {
public:
    AstFunCall(StringRef funName, const BoundArray<AstExp*>& args)
        : AstExp(Kind::FunCall)
        , funName(funName)
        , args(args) {  }

    StringRef getFunName() const { return funName; }
    BoundArray<AstExp*>& getArgs() { return args; }

private:
    StringRef funName;
    BoundArray<AstExp*> args;
};