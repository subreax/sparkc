#pragma once
#include "sparkc/common/StringRef.h"
#include "sparkc/common/cfg/CfGraph.h"
#include "sparkc/skr/instr/SkrInstruction.h"

class SkrOptOnCfgCreatedListener {
public:
    virtual ~SkrOptOnCfgCreatedListener() = default;
    virtual void onCfgCreated(StringRef funName, int iteration, CfGraph<SkrInstruction*>* graph) = 0;
};
