#pragma once
#include <vector>
#include <unordered_set>
#include "CfGraph.h"

template<typename I>
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
        int counter = 0;
        nodes.emplace_back(new CfgBlock<I*>(counter++));

        std::vector<I*> block;
        for (auto* instr : body) {
            if (!block.empty() && cfg::isLabel(instr)) {
                nodes.emplace_back(new CfgBlock<I*>(counter++, block));
                block.clear();
            }

            block.emplace_back(instr);
            if (cfg::isJump(instr) || cfg::isBranch(instr)) {
                nodes.emplace_back(new CfgBlock<I*>(counter++, block));
                block.clear();
            }
        }

        if (!block.empty()) {
            nodes.emplace_back(new CfgBlock<I*>(counter++, block));
        }

        nodes.emplace_back(new CfgBlock<I*>(counter++));
    }

    void addEdges() {
        auto& blocks = graph->getNodes(); 

        for (size_t i = 0; i < blocks.size() - 1; i++) {
            CfgBlock<SkrInstruction*>* block = blocks[i];
            if (isVisited(block)) {
                continue;
            }

            visited.emplace(block->getId());

            if (block->hasJump()) {
                const char* jumpLabel = block->getJumpOrBranchLabel();
                auto* nextNode = findBlockByLabel(jumpLabel);
                graph->connect(block, nextNode);
            }
            else if (block->hasBranch()) {
                const char* jumpLabel = block->getJumpOrBranchLabel();
                auto* jumpNode = findBlockByLabel(jumpLabel);
                graph->connect(block, jumpNode);
                graph->connect(block, blocks[i + 1]);
            }
            else {
                graph->connect(block, blocks[i + 1]);
            }
        }
    }

    CfgBlock<I*>* findBlockByLabel(const char* label) {
        const auto& nodes = graph->getNodes();
        for (auto* node : nodes) {
            if (node->isEmpty() || !node->isLabeled()) {
                continue;
            }

            const char* nodeLabel = node->getLabel();
            if (strncmp(label, nodeLabel, 128) == 0) {
                return node;
            }
        }

        sparkError("CfgBuilder", "Failed to find block by label: %s", label);
        return nullptr;
    }

    bool isVisited(const CfgBlock<I*>* block) {
        return visited.find(block->getId()) != visited.end();
    }


    std::unordered_set<int> visited;
    CfGraph<I*>* graph = nullptr;
};