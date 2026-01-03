#pragma once
#include "CfGraph.h"
#include <unordered_set>
#include <vector>

template <typename I>
class CfgBuilder {
public:
    CfGraph<I*>* build_delGraphWhenDone(const std::vector<I*>& body) {
        std::vector<CfgBlock<I*>*> nodes;
        readBlocks(body, nodes);
        graph = new CfGraph<I*>(nodes);
        addEdges();
        return graph;
    }

    CfGraph<I*>& getGraph() {
        if (graph == nullptr) {
            sparkError("CfgBuilder", "Graph is not ready");
        }
        return *graph;
    }

private:
    static void readBlocks(const std::vector<I*>& body, std::vector<CfgBlock<I*>*>& nodes) {
        int blockIdx = 0;
        nodes.emplace_back(new CfgBlock<I*>(blockIdx++));

        std::vector<I*> block;
        for (auto* instr : body) {
            if (!block.empty() && cfg::isLabel(instr)) {
                nodes.emplace_back(new CfgBlock<I*>(blockIdx++, block));
                block.clear();
            }

            block.emplace_back(instr);
            if (cfg::isJump(instr) || cfg::isBranch(instr)) {
                nodes.emplace_back(new CfgBlock<I*>(blockIdx++, block));
                block.clear();
            }
        }

        if (!block.empty()) {
            nodes.emplace_back(new CfgBlock<I*>(blockIdx++, block));
        }

        nodes.emplace_back(new CfgBlock<I*>(blockIdx++));
    }

    void addEdges() {
        const auto& blocks = graph->getBlocks();
        std::unordered_set<int> visited;

        for (size_t i = 0; i < blocks.size() - 1; i++) {
            CfgBlock<SkrInstruction*>* block = blocks[i];
            if (isVisited(visited, block)) {
                continue;
            }

            visited.emplace(block->getIdx());

            if (block->hasJump()) {
                auto jumpLabel = block->getJumpOrBranchLabel();
                auto* nextNode = findBlockByLabel(jumpLabel);
                graph->connect(block, nextNode);
            }
            else if (block->hasBranch()) {
                auto jumpLabel = block->getJumpOrBranchLabel();
                auto* jumpNode = findBlockByLabel(jumpLabel);
                graph->connect(block, jumpNode);
                graph->connect(block, blocks[i + 1]);
            }
            else {
                graph->connect(block, blocks[i + 1]);
            }
        }
    }

    CfgBlock<I*>* findBlockByLabel(StringRef label) {
        const auto& nodes = graph->getBlocks();
        for (auto* node : nodes) {
            if (node->isEmpty() || !node->isLabeled()) {
                continue;
            }

            StringRef nodeLabel = node->getLabel();
            if (label == nodeLabel) {
                return node;
            }
        }

        sparkError("CfgBuilder", "Failed to find block by label: %s", label);
        return nullptr;
    }

    static bool isVisited(const std::unordered_set<int>& visited, const CfgBlock<I*>* block) {
        return visited.find(block->getIdx()) != visited.end();
    }

    CfGraph<I*>* graph = nullptr;
};