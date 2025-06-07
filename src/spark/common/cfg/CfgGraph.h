#pragma once
#include "CfgBlock.h"
#include "BitMatrix.h"
#include "../Error.h"


template<typename CfgGraph>
class CfgSGraphIterator {
public:
    using ItemType = typename CfgGraph::ItemType;

    CfgSGraphIterator(const std::vector<CfgBlock<ItemType>*>& nodes, const BitMatrix& edges, size_t nodeIdx, size_t c = -1)
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

    CfgSGraphIterator& operator++() {
        do {
            c = std::min(c + 1, edges.getColsCount());
        } while (c < edges.getColsCount() && !edges.get(r, c));
        return *this;
    }

    bool operator==(const CfgSGraphIterator& other) const {
        return c == other.c;
    }

    bool operator!=(const CfgSGraphIterator& other) const {
        return !(*this == other);
    }

private:
    size_t r;
    size_t c = -1;

    const std::vector<CfgBlock<ItemType>*>& nodes;
    const BitMatrix& edges;
};

template<typename CfgGraph>
class CfgPGraphIterator {
public:
    using ItemType = typename CfgGraph::ItemType;

    CfgPGraphIterator(const std::vector<CfgBlock<ItemType>*>& nodes, const BitMatrix& edges, size_t nodeIdx, size_t r = -1)
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

    CfgPGraphIterator& operator++() {
        do {
            r = std::min(r + 1, edges.getRowsCount());
        } while (r < edges.getRowsCount() && !edges.get(r, c));
        return *this;
    }

    bool operator==(const CfgPGraphIterator& other) const {
        return r == other.r;
    }

    bool operator!=(const CfgPGraphIterator& other) const {
        return !(*this == other);
    }

private:
    size_t r = -1;
    size_t c;

    const std::vector<CfgBlock<ItemType>*>& nodes;
    const BitMatrix& edges;
};

template<typename I>
class CfgGraph {
public:
    using ItemType = I;
    using PIterator = CfgPGraphIterator<CfgGraph>;
    using SIterator = CfgSGraphIterator<CfgGraph>;

    CfgGraph(const std::vector<CfgBlock<I>*>& nodes)
        : nodes(nodes)
        , edges(nodes.size()) {  }


    ~CfgGraph() {
        for (auto* node : nodes) {
            delete node;
        }
    }

    void connect(CfgBlock<I>* block1, CfgBlock<I>* block2) {
        size_t p1 = indexOf(block1);
        size_t p2 = indexOf(block2);
        edges.set(p1, p2);
    }

    bool isConnected(const CfgBlock<I>* block1, const CfgBlock<I>* block2) const {
        return edges.get(indexOf(block1), indexOf(block2));
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

private:
    size_t indexOf(const CfgBlock<I>* block) const {
        for (size_t i = 0; i < nodes.size(); i++) {
            if (nodes[i] == block) {
                return i;
            }
        }

        sparkError("CfgGraph", "indexOf failed");
        return 0;
    }

    std::vector<CfgBlock<I>*> nodes;
    BitMatrix edges;
};
