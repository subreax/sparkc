#pragma once
#include "CfgBlock.h"
#include "BitMatrix.h"
#include "../Error.h"


template<typename CfgGraph>
class CfSGraphIterator {
public:
    using ItemType = typename CfgGraph::ItemType;

    CfSGraphIterator(const std::vector<CfgBlock<ItemType>*>& nodes, const BitMatrix& edges, size_t nodeIdx, size_t c = -1)
        : nodes(nodes)
        , edges(edges)
        , r(nodeIdx) 
        , c(c)
    {
        if (c == -1) {
            operator++();
        }
    }

    CfgBlock<ItemType>* operator*() {
        return nodes[c];
    }

    CfSGraphIterator& operator++() {
        do {
            c = std::min(c + 1, edges.getColsCount());
        } while (c < edges.getColsCount() && !edges.get(r, c));
        return *this;
    }

    bool operator==(const CfSGraphIterator& other) const {
        return c == other.c;
    }

    bool operator!=(const CfSGraphIterator& other) const {
        return !(*this == other);
    }

private:
    size_t r;
    size_t c = -1;

    const std::vector<CfgBlock<ItemType>*>& nodes;
    const BitMatrix& edges;
};

template<typename CfgGraph>
class CfPGraphIterator {
public:
    using ItemType = typename CfgGraph::ItemType;

    CfPGraphIterator(const std::vector<CfgBlock<ItemType>*>& nodes, const BitMatrix& edges, size_t nodeIdx, size_t r = -1)
        : nodes(nodes)
        , edges(edges)
        , r(r)
        , c(nodeIdx) 
    {
        operator++();
    }

    CfgBlock<ItemType>* operator*() const {
        return nodes[r];
    }

    CfPGraphIterator& operator++() {
        do {
            r = std::min(r + 1, edges.getRowsCount());
        } while (r < edges.getRowsCount() && !edges.get(r, c));
        return *this;
    }

    bool operator==(const CfPGraphIterator& other) const {
        return r == other.r;
    }

    bool operator!=(const CfPGraphIterator& other) const {
        return !(*this == other);
    }

private:
    size_t r = -1;
    size_t c;

    const std::vector<CfgBlock<ItemType>*>& nodes;
    const BitMatrix& edges;
};

template<typename I>
class CfGraph {
public:
    using ItemType = I;
    using PIterator = CfPGraphIterator<CfGraph>;
    using SIterator = CfSGraphIterator<CfGraph>;

    CfGraph(const std::vector<CfgBlock<I>*>& nodes)
        : nodes(nodes)
        , edges(nodes.size()) {  }


    ~CfGraph() {
        for (auto* node : nodes) {
            delete node;
        }
    }

    void connect(CfgBlock<I>* block1, CfgBlock<I>* block2) {
        size_t p1 = indexOf(block1);
        size_t p2 = indexOf(block2);
        edges.set(p1, p2);
    }

    void disconnect(CfgBlock<I>* block) {
        size_t idx = indexOf(block);
        size_t edgesCount = edges.getRowsCount(); // doesn't matter
        for (size_t i = 0; i < edgesCount; i++) {
            edges.remove(idx, i);
            edges.remove(i, idx);
        }
    }

    size_t countSuccessors(const CfgBlock<I>* block) const {
        auto it = successorsIterator(block);
        auto end = sEnd();
        size_t count = 0;
        while (it != end) {
            ++count;
            ++it;
        }
        return count;
    }

    size_t countPrecedessors(const CfgBlock<I>* block) const {
        auto it = precedessorsIterator(block);
        auto end = pEnd();
        size_t count = 0;
        while (it != end) {
            ++count;
            ++it;
        }
        return count;
    }

    bool isConnected(const CfgBlock<I>* block1, const CfgBlock<I>* block2) const {
        return edges.get(indexOf(block1), indexOf(block2));
    }

    bool hasConnections(const CfgBlock<I>* block) const {
        size_t idx = indexOf(block);
        size_t edgesCount = edges.getRowsCount();
        for (size_t i = 0; i < edgesCount; i++) {
            if (edges.get(idx, i) + edges.get(i, idx) > 0) {
                return true;
            }
        }
        return false;
    }

    SIterator successorsIterator(const CfgBlock<I>* block) const {
        return SIterator(nodes, edges, indexOf(block));
    }

    SIterator sEnd() const {
        return SIterator(nodes, edges, nodes.size(), nodes.size());
    }

    PIterator precedessorsIterator(const CfgBlock<I>* block) const {
        return PIterator(nodes, edges, indexOf(block));
    }

    PIterator pEnd() const {
        return PIterator(nodes, edges, nodes.size(), nodes.size());
    }

    const std::vector<CfgBlock<I>*>& getNodes() const {
        return nodes;
    }

    void toPlain(std::vector<I>& out) {
        for (CfgBlock<I>* block : nodes) {
            if (hasConnections(block)) {
                block->copyTo(out);
            }
        }
    }

private:
    size_t indexOf(const CfgBlock<I>* block) const {
        for (size_t i = 0; i < nodes.size(); i++) {
            if (nodes[i] == block) {
                return i;
            }
        }

        sparkError("CfGraph", "indexOf failed");
        return 0;
    }

    std::vector<CfgBlock<I>*> nodes;
    BitMatrix edges;
};
