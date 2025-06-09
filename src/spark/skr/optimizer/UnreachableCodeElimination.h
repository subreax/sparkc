#pragma once
#include <unordered_set>
#include "SkrCfgOverloadings.h"
#include "../../common/cfg/CfGraph.h"
#include "../instr/everything.h"

class UnreachableCodeElimination {
public:
    UnreachableCodeElimination(CfGraph<SkrInstruction*>* graph)
        : graph(graph) {  }

    void run() {
        disconnectUnreachableBlocks();
        removeUselessJumps();
        removeUselessLabels();
    }

private:
    void disconnectUnreachableBlocks() {
        std::unordered_set<int> visited;
        auto& nodes = graph->getNodes();
        traverse(nodes.front(), visited);

        for (auto* block : nodes) {
            if (!contains(visited, block)) {
                graph->disconnect(block);
            }
        }
    }

    void removeUselessJumps() {
        const auto& nodes = graph->getNodes();
        if (nodes.empty()) {
            return;
        }

        for (size_t i = 0; i < nodes.size() - 1; i++) {
            auto* curr = nodes[i];
            auto* next = nodes[i + 1];

            if (graph->isConnected(curr, next) && curr->hasJump() && next->isLabeled()) {
                auto* jumpLabel = curr->getJumpOrBranchLabel();
                auto* blockLabel = next->getLabel();
                if (strncmp(jumpLabel, blockLabel, 128) == 0) {
                    curr->getBody().erase(curr->getBody().end() - 1);
                }
            }
        }
    }

    void removeUselessLabels() {
        const auto& nodes = graph->getNodes();
        if (nodes.empty()) {
            return;
        }

        for (size_t i = 0; i < nodes.size() - 1; i++) {
            auto* curr = nodes[i];
            auto* next = nodes[i + 1];

            if (graph->isConnected(curr, next) && next->isLabeled() && graph->countPrecedessors(next) == 1) {
                next->getBody().erase(next->getBody().begin());
            }
        }
    }

    void traverse(const CfgBlock<SkrInstruction*>* block, std::unordered_set<int>& visited) {
        if (contains(visited, block)) {
            return;
        }

        visited.emplace(block->getId());

        auto it = graph->successorsIterator(block);
        auto end = graph->sEnd();

        while (it != end) {
            traverse(*it, visited);
            ++it;
        }
    }

    static bool contains(const std::unordered_set<int>& s, const CfgBlock<SkrInstruction*>* block) {
        return s.find(block->getId()) != s.end();
    }

    CfGraph<SkrInstruction*>* graph;
};
