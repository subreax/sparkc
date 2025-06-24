#pragma once
#include <vector>
#include "instr/SkrInstruction.h"
#include "value/SkrValue.h"
#include "../common/BoundArray.h"

class SkrFunction {
public:
    SkrFunction(const char* name, BoundArray<SkrVar*> params, BoundArray<SkrInstruction*> instructions, const SkrVar* retVar)
        : instructions(instructions)
        , params(params)
        , retVar(retVar)
        , name(name) {  }

    const char* getName() const { return name; }
    const BoundArray<SkrVar*>& getParams() const { return params; }
    const SkrVar* getRetVar() const { return retVar; }
    BoundArray<SkrInstruction*>& getInstructions() { return instructions; }

private:
    BoundArray<SkrInstruction*> instructions;
    BoundArray<SkrVar*> params;
    const SkrVar* retVar;
    const char* name;
};