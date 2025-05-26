#pragma once
#include "SkrInstruction.h"
#include "../value/SkrValue.h"
#include "../../common/BoundArray.h"

class SkrFunCall : public SkrInstruction {
public:
    SkrFunCall(const char* name, BoundArray<SkrValue*> args, SkrVar* retVar)
        : SkrInstruction(Kind::FunCall)
        , name(name)
        , retVar(retVar)
        , args(args) {  }

    const char* getName() const { return name; }
    const SkrVar* getRetVar() const { return retVar; }
    const BoundArray<SkrValue*>& getArgs() const { return args; }

private:
    const char* name;
    SkrVar* retVar;
    BoundArray<SkrValue*> args;
};