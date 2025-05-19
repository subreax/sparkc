#pragma once
#include <vector>
#include "instr/SkrInstruction.h"

class SkrFunction {
public:
    SkrFunction(const char* name, const char* resultId, const std::vector<SkrInstruction*>& instructions) 
        : instructions(instructions), resultId(resultId), name(name) {  }

    const char* getName() const { return name; }
    std::vector<SkrInstruction*>& getInstructions() { return instructions; }

private:
    std::vector<SkrInstruction*> instructions;
    const char* resultId;
    const char* name;
};