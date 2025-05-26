#pragma once
#include <vector>
#include "instr/SkrInstruction.h"
#include "value/SkrValue.h"
#include "../common/BoundArray.h"

class SkrFunction {
public:
    SkrFunction(const char* name, BoundArray<SkrVar*> params, const std::vector<SkrInstruction*>& instructions, const char* resultId)
        : instructions(instructions)
        , params(params)
        , resultId(resultId)
        , name(name) {  }

    const char* getName() const { return name; }
    const BoundArray<SkrVar*>& getParams() const { return params; }
    const char* getResultIdentifier() const { return resultId; }
    std::vector<SkrInstruction*>& getInstructions() { return instructions; }

private:
    std::vector<SkrInstruction*> instructions;
    BoundArray<SkrVar*> params;
    const char* resultId;
    const char* name;
};