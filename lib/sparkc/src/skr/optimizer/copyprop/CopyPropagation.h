#pragma once
#include <vector>
#include "sparkc/skr/optimizer/SkrCfg.h"
#include "sparkc/skr/instr/SkrCopy.h"
#include "sparkc/skr/SkrFactory.h"
#include "ReachingCopies.h"

class CopyPropagation {
public:
    CopyPropagation(SkrFactory& skrf, SkrCfg& graph);
    void run();

private:
    SkrInstruction* rewriteInstruction(
        SkrInstruction* instr,
        const ReachingCopies& reachingCopies
    );

    SkrInstruction* rewriteInstruction(
        SkrCfgBlock& block,
        class RCABlock& annotated,
        size_t instrIdx
    );

    std::vector<SkrValue*> replaceArgs(
        const BoundArray<SkrValue*>& args,
        const ReachingCopies& copies
    );

    static SkrValue* replace(SkrValue* value, const ReachingCopies& copies);

    SkrFactory& skrf;
    SkrCfg& graph;
};
