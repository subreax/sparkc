#pragma once
#include "../../common/StringRef.h"
#include "../../common/cfg/CfGraph.h"
#include "../instr/SkrInstruction.h"

class SkrOptOnCfgCreatedListener {
public:
    virtual ~SkrOptOnCfgCreatedListener() = default;
    virtual void onCfgCreated(StringRef funName, int iteration, CfGraph<SkrInstruction*>* graph) = 0;
};
