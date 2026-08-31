#pragma once
#include <vector>
#include "CFG.h"

template <typename I>
class CfgBuilder {
public:
    static CFG<I> build(const std::vector<I>& instructions) {
        CFG<I> graph(readBlocks(instructions));
        addEdges(graph);
        return graph;
    }

private:
    static std::vector<CfgBlock<I>> readBlocks(const std::vector<I>& instructions) {
        std::vector<CfgBlock<I>> blocks;

        int blockIdx = 0;
        blocks.emplace_back(blockIdx++); // start block

        std::vector<I> block;
        for (const auto& instr : instructions) {
            if (!block.empty() && cfg::isLabel(instr)) {
                blocks.emplace_back(blockIdx++, block);
                block.clear();
            }

            block.emplace_back(instr);

            if (cfg::isJump(instr) || cfg::isBranch(instr)) {
                blocks.emplace_back(blockIdx++, block);
                block.clear();
            }
        }

        if (!block.empty()) {
            blocks.emplace_back(blockIdx++, block);
        }

        blocks.emplace_back(blockIdx++); // end block
        return blocks;
    }

    static void addEdges(CFG<I>& graph) {
        for (size_t i = 0; i < graph.getSize() - 1; i++) {
            CfgBlock<I>& block = graph[i];
            auto blockIdx = i;

            if (block.hasJump()) {
                auto jumpLabel = block.getJumpOrBranchLabel();
                auto nextNodeIdx = findBlockIdxByLabel(graph, jumpLabel);
                graph.connect(blockIdx, nextNodeIdx);
            }
            else if (block.hasBranch()) {
                auto jumpLabel = block.getJumpOrBranchLabel();
                auto jumpNodeIdx = findBlockIdxByLabel(graph, jumpLabel);
                graph.connect(blockIdx, jumpNodeIdx);
                graph.connect(blockIdx, i + 1);
            }
            else {
                graph.connect(blockIdx, i + 1);
            }
        }
    }

    static size_t findBlockIdxByLabel(const CFG<I>& graph, StringRef label) {
        for (const auto& block : graph.getNodes()) {
            if (block.isEmpty() || !block.isLabeled()) {
                continue;
            }

            StringRef blockLabel = block.getLabel();
            if (label == blockLabel) {
                return block.getIdx();
            }
        }

        sparkError("CfgBuilder", "Failed to find block with label '%s'", label);
        return -1;
    }
};