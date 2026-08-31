#pragma once
#include <vector>
#include <optional>
#include "../Error.h"
#include "BitMatrix.h"

template <typename T>
class Graph {
public:
    class SuccessorFinder {
    public:
        static size_t findNext(const Graph<T>& graph, size_t nodeIdx, size_t from) {
            size_t successorNodeIdx = from;
            while (successorNodeIdx < graph.getSize()) {
                if (graph.edges.get(nodeIdx, successorNodeIdx)) {
                    break;
                }
                successorNodeIdx++;
            }

            return successorNodeIdx;
        }
    };

    class PredecessorFinder {
    public:
        static size_t findNext(const Graph<T>& graph, size_t nodeIdx, size_t from) {
            size_t predecessorNodeIdx = from;
            while (predecessorNodeIdx < graph.getSize()) {
                if (graph.edges.get(predecessorNodeIdx, nodeIdx)) {
                    break;
                }
                predecessorNodeIdx++;
            }
            return predecessorNodeIdx;
        }
    };

    template <typename NodeFinder, bool IsConst>
    class IteratorBase {
    public:
        using GraphRef = std::conditional_t<IsConst, const Graph<T>&, Graph<T>&>;
        using NodeRef = std::conditional_t<IsConst, const T&, T&>;

        IteratorBase(GraphRef graph, size_t nodeIdx)
            : graph(graph)
            , nodeIdx(nodeIdx) {
            moveToNext();
        }

        bool hasNext() const {
            return nextNodeIdx < graph.getSize();
        }

        std::pair<size_t, NodeRef> next() {
            if (!hasNext()) {
                sparkError("Graph::Iterator", "Can't go next");
            }
            size_t idx = nextNodeIdx;
            NodeRef val = graph.nodes[idx];
            moveToNext();
            return { idx, val };
        }

        size_t nextIdx() {
            return next().first;
        }

        NodeRef nextNode() {
            return next().second;
        }

    private:
        void moveToNext() {
            nextNodeIdx = NodeFinder::findNext(graph, nodeIdx, nextNodeIdx + 1);
        }

        GraphRef graph;
        const size_t nodeIdx;
        size_t nextNodeIdx = -1;
    };

    using SuccessorsIterator = IteratorBase<SuccessorFinder, false>;
    using CSuccessorsIterator = IteratorBase<SuccessorFinder, true>;
    using PredecessorsIterator = IteratorBase<PredecessorFinder, false>;
    using CPredecessorsIterator = IteratorBase<PredecessorFinder, true>;

    Graph(size_t nodesCount)
        : edges(nodesCount)
        , sz(nodesCount)
        , nodes(nodesCount) { }

    Graph(std::vector<T> nodes)
        : edges(nodes.size())
        , sz(nodes.size())
        , nodes(std::move(nodes)) { }

    void setNode(size_t idx, const T& node) {
        checkIdx(idx);
        nodes[idx] = node;
    }

    T& getNode(size_t idx) {
        checkIdx(idx);
        return nodes[idx];
    }

    T& operator[](size_t idx) {
        return getNode(idx);
    }

    const T& getNode(size_t idx) const {
        checkIdx(idx);
        return nodes[idx];
    }

    const T& operator[](size_t idx) const {
        return getNode(idx);
    }

    void connect(size_t nodeIdx1, size_t nodeIdx2) {
        checkIdx(nodeIdx1);
        checkIdx(nodeIdx2);
        edges.set(nodeIdx1, nodeIdx2);
    }

    void disconnect(size_t nodeIdx1, size_t nodeIdx2) {
        checkIdx(nodeIdx1);
        checkIdx(nodeIdx2);
        edges.remove(nodeIdx1, nodeIdx2);
    }

    void disconnect(size_t nodeIdx) {
        checkIdx(nodeIdx);

        size_t edgesCount = sz;
        for (size_t i = 0; i < edgesCount; i++) {
            edges.remove(nodeIdx, i);
            edges.remove(i, nodeIdx);
        }
    }

    bool isConnected(size_t nodeIdx1, size_t nodeIdx2) const {
        checkIdx(nodeIdx1);
        checkIdx(nodeIdx2);
        return edges.get(nodeIdx1, nodeIdx2);
    }

    bool hasAnyConnections(size_t nodeIdx) const {
        checkIdx(nodeIdx);

        size_t edgesCount = edges.getRowsCount();
        for (size_t i = 0; i < edgesCount; i++) {
            if (edges.get(nodeIdx, i) + edges.get(i, nodeIdx) > 0) {
                return true;
            }
        }
        return false;
    }

    size_t getSize() const {
        return sz;
    }

    bool isEmpty() const {
        return nodes.empty();
    }

    const std::vector<T>& getNodes() const {
        return nodes;
    }

    PredecessorsIterator predecessors(size_t nodeIdx) {
        checkIdx(nodeIdx);
        return PredecessorsIterator(*this, nodeIdx);
    }

    CPredecessorsIterator predecessors(size_t nodeIdx) const {
        checkIdx(nodeIdx);
        return CPredecessorsIterator(*this, nodeIdx);
    }

    SuccessorsIterator successors(size_t nodeIdx) {
        checkIdx(nodeIdx);
        return SuccessorsIterator(*this, nodeIdx);
    }

    CSuccessorsIterator successors(size_t nodeIdx) const {
        checkIdx(nodeIdx);
        return CSuccessorsIterator(*this, nodeIdx);
    }

    size_t countPredecessors(size_t nodeIdx) const {
        size_t count = 0;
        auto it = predecessors(nodeIdx);
        while (it.hasNext()) {
            it.nextIdx();
            count++;
        }
        return count;
    }

    size_t countSuccessors(size_t nodeIdx) const {
        size_t count = 0;
        auto it = successors(nodeIdx);
        while (it.hasNext()) {
            it.nextIdx();
            count++;
        }
        return count;
    }

private:
    inline void checkIdx(size_t idx) const {
        if (idx >= sz) {
            sparkError("Graph", "Index is out of bounds");
        }
    }

    BitMatrix edges;
    const size_t sz;
    std::vector<T> nodes;
};
