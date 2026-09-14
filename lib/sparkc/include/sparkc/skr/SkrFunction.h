#pragma once
#include "SkrProgItem.h"
#include "sparkc/common/BoundArray.h"
#include "instr/SkrInstruction.h"
#include "value/SkrValue.h"
#include <vector>

class SkrFunction : public SkrProgItem {
public:
    SkrFunction(
        StringRef name,
        BoundArray<SkrVar*> params,
        BoundArray<SkrInstruction*> instructions,
        const SkrVar* retVar
    )
        : SkrProgItem(Kind::Function)
        , instructions(instructions)
        , params(params)
        , retVar(retVar)
        , name(name) { }

    StringRef getName() const { return name; }
    const BoundArray<SkrVar*>& getParams() const { return params; }
    const SkrVar* getRetVar() const { return retVar; }
    BoundArray<SkrInstruction*>& getInstructions() { return instructions; }

private:
    BoundArray<SkrInstruction*> instructions;
    BoundArray<SkrVar*> params;
    const SkrVar* retVar;
    StringRef name;
};