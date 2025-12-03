#pragma once
#include <vector>
#include "sparkc/common/cfg/CfGraph.h"
#include "sparkc/skr/instr/SkrCopy.h"

class CopyPropagation {
public:
    CopyPropagation(CfGraph<SkrInstruction*>* graph, Allocator& allocator);
    void run();

private:
    SkrInstruction* rewriteInstruction(
        SkrInstruction* instr, 
        const std::vector<SkrCopy*>& reachingCopies
    );

    SkrInstruction* rewriteInstruction(
        CfgBlock<SkrInstruction*>* block, 
        class RCABlock* annotated, 
        size_t instrIdx
    );

    void replace(const BoundArray<SkrValue*>& values, const std::vector<SkrCopy*>& copies, std::vector<SkrValue*>& out);

    static SkrValue* replace(SkrValue* value, const std::vector<SkrCopy*>& copies);
    static bool shouldBeRemoved(SkrInstruction* skr);

    CfgBlock<SkrInstruction*>* getBlock(int idx);
    const CfgBlock<SkrInstruction*>* getBlock(int idx) const;

    static SkrCopy* findCopyByDst(const std::vector<SkrCopy*>& copies, SkrVar* var);

    CfGraph<SkrInstruction*>* graph;
    Allocator& allocator;
};
