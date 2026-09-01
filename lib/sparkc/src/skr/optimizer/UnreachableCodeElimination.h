#pragma once
#include <unordered_set>
#include "sparkc/skr/instr/everything.h"
#include "sparkc/common/cfg/CFG.h"

class UnreachableCodeElimination {
public:
    UnreachableCodeElimination(SkrCfg& graph)
        : graph(graph) { }

    void run() {
        disconnectUnreachableBlocks();

        auto aliveBlocks = getAliveBlocks();
        removeUselessJumps(aliveBlocks);
        removeUselessLabels(aliveBlocks);
    }

private:
    void disconnectUnreachableBlocks() {
        BitArray visited(graph.getSize());
        traverse(0, visited);

        for (size_t i = 0; i < graph.getSize(); i++) {
            if (!visited[i]) {
                graph.disconnect(i);
            }
        }
    }

    void removeUselessJumps(const std::vector<size_t>& aliveBlocks) {
        if (aliveBlocks.empty()) {
            return;
        }

        for (size_t i = 0; i < aliveBlocks.size() - 1; i++) {
            size_t currIdx = aliveBlocks[i];
            auto& curr = graph[currIdx];
            size_t nextIdx = currIdx + 1;
            auto& next = graph[nextIdx];

            if (graph.isConnected(currIdx, nextIdx) && curr.hasJump() && next.isLabeled()) {
                auto jumpLabel = curr.getJumpOrBranchLabel();
                auto nextLabel = next.getLabel();
                if (jumpLabel == nextLabel) {
                    eraseJump(curr);
                }
            }
        }
    }

    void removeUselessLabels(const std::vector<size_t>& aliveBlocks) {
        if (aliveBlocks.empty()) {
            return;
        }

        for (size_t i = 0; i < aliveBlocks.size() - 1; i++) {
            size_t currIdx = aliveBlocks[i];
            auto& curr = graph[currIdx];
            size_t nextIdx = currIdx + 1;
            auto& next = graph[nextIdx];

            if (graph.isConnected(currIdx, nextIdx) && next.isLabeled() && hasSinglePredecessor(nextIdx)) {
                eraseLabel(next);
            }
        }
    }

    void traverse(size_t block, BitArray& visited) {
        if (visited[block]) {
            return;
        }

        visited.set(block);

        auto it = graph.successors(block);
        while (it.hasNext()) {
            traverse(it.nextIdx(), visited);
        }
    }

    bool hasSinglePredecessor(size_t blockIdx) const {
        auto it = graph.predecessors(blockIdx);
        if (!it.hasNext()) {
            return false;
        }
        it.next();
        return !it.hasNext();
    }

    void eraseLabel(SkrCfgBlock& block) {
        block.eraseFirstInstruction();
    }

    void eraseJump(SkrCfgBlock& block) {
        block.eraseLastInstruction();
    }

    std::vector<size_t> getAliveBlocks() const {
        std::vector<size_t> aliveBlocks;
        aliveBlocks.reserve(graph.getSize());
        for (size_t i = 0; i < graph.getSize(); i++) {
            if (graph.hasAnyConnections(i)) {
                aliveBlocks.emplace_back(i);
            }
        }
        return aliveBlocks;
    }

    SkrCfg& graph;
};
