#pragma once
#include "../../common/BoundArray.h"
#include "../value/SkrValue.h"
#include "SkrInstruction.h"

class SkrFunCall : public SkrInstruction {
public:
    SkrFunCall(StringRef name, BoundArray<SkrValue*> args, const SkrVar* retVar)
        : SkrInstruction(Kind::FunCall)
        , name(name)
        , retVar(retVar)
        , args(args) { }

    StringRef getName() const { return name; }
    const SkrVar* getRetVar() const { return retVar; }
    const BoundArray<SkrValue*>& getArgs() const { return args; }

private:
    StringRef name;
    const SkrVar* retVar;
    BoundArray<SkrValue*> args;
};