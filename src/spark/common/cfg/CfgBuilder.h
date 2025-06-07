#pragma once
#include <vector>
#include <unordered_set>
#include "CfgGraph.h"

template<typename I>
class CfgBuilder {
public:
    CfgGraph<I*>* build_delGraphWhenDone(const std::vector<I*>& body) {
        std::vector<CfgBlock<I*>*> nodes;
        readBlocks(body, nodes);
        graph = new CfgGraph<I*>(nodes);
        addEdges();
        return graph;
    }

    CfgGraph<I*>& getGraph() {
        if (graph == nullptr) {
            sparkError("CfgBuilder", "Graph is not ready");
        }
        return *graph;
    }

private:
    void readBlocks(const std::vector<I*>& body, std::vector<CfgBlock<I*>*>& nodes) {
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
            auto* block = blocks[i];
            if (isVisited(block)) {
                continue;
            }

            visited.emplace(block->getId());

            if (hasJump(block)) {
                const char* jumpLabel = cfg::getLabel(block->getBody().back());
                auto* nextNode = findBlockByLabel(jumpLabel);
                graph->connect(block, nextNode);
            }
            else if (hasBranch(block)) {
                const char* jumpLabel = cfg::getLabel(block->getBody().back());
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
            if (node->isEmpty()) {
                continue;
            }

            auto* firstInstr = node->getBody().front();
            if (!cfg::isLabel(firstInstr)) {
                continue;
            }

            const char* nodeLabel = cfg::getLabel(firstInstr);
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

    bool hasJump(CfgBlock<I*>* block) {
        return block->isNotEmpty() && cfg::isJump(block->getBody().back());
    }

    bool hasBranch(CfgBlock<I*>* block) {
        return block->isNotEmpty() && cfg::isBranch(block->getBody().back());
    }


    std::unordered_set<int> visited;
    CfgGraph<I*>* graph = nullptr;
};