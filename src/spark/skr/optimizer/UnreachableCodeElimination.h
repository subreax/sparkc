#pragma once
#include <unordered_set>
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
        auto& blocks = graph->getBlocks();
        traverse(blocks.front(), visited);

        for (auto* block : blocks) {
            if (!contains(visited, block)) {
                graph->disconnect(block);
            }
        }
    }

    void removeUselessJumps() {
        const auto& blocks = graph->getBlocks();
        if (blocks.empty()) {
            return;
        }

        for (size_t i = 0; i < blocks.size() - 1; i++) {
            auto* curr = blocks[i];
            auto* next = blocks[i + 1];

            if (graph->isConnected(curr, next) && curr->hasJump() && next->isLabeled()) {
                auto jumpLabel = curr->getJumpOrBranchLabel();
                auto blockLabel = next->getLabel();
                if (jumpLabel == blockLabel) {
                    curr->getBody().erase(curr->getBody().end() - 1);
                }
            }
        }
    }

    void removeUselessLabels() {
        const auto& blocks = graph->getBlocks();
        if (blocks.empty()) {
            return;
        }

        for (size_t i = 0; i < blocks.size() - 1; i++) {
            auto* curr = blocks[i];
            auto* next = blocks[i + 1];

            if (graph->isConnected(curr, next) && next->isLabeled() && graph->countPrecedessors(next) == 1) {
                next->getBody().erase(next->getBody().begin());
            }
        }
    }

    void traverse(const CfgBlock<SkrInstruction*>* block, std::unordered_set<int>& visited) {
        if (contains(visited, block)) {
            return;
        }

        visited.emplace(block->getIdx());

        auto it = graph->successorsIterator(block);
        auto end = graph->sEnd();
        while (it != end) {
            traverse(*it, visited);
            ++it;
        }
    }

    static bool contains(const std::unordered_set<int>& s, const CfgBlock<SkrInstruction*>* block) {
        return s.find(block->getIdx()) != s.end();
    }

    CfGraph<SkrInstruction*>* graph;
};
