#pragma once
#include "../Error.h"
#include "BitMatrix.h"
#include "CfgBlock.h"

template <typename CfgGraph>
class CfSGraphIterator {
public:
    using ItemType = typename CfgGraph::ItemType;

    CfSGraphIterator(
        const std::vector<CfgBlock<ItemType>*>& nodes,
        const BitMatrix& edges,
        size_t nodeIdx,
        size_t c = -1
    )
        : nodes(nodes)
        , edges(edges)
        , r(nodeIdx)
        , c(c) {
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

template <typename CfgGraph>
class CfPGraphIterator {
public:
    using ItemType = typename CfgGraph::ItemType;

    CfPGraphIterator(
        const std::vector<CfgBlock<ItemType>*>& nodes,
        const BitMatrix& edges,
        size_t nodeIdx,
        size_t r = -1
    )
        : nodes(nodes)
        , edges(edges)
        , r(r)
        , c(nodeIdx) {
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

template <typename I>
class CfGraph {
public:
    using ItemType = I;
    using PIterator = CfPGraphIterator<CfGraph>;
    using SIterator = CfSGraphIterator<CfGraph>;

    CfGraph(const std::vector<CfgBlock<I>*>& nodes)
        : blocks(nodes)
        , edges(nodes.size()) { }

    ~CfGraph() {
        for (auto* node : blocks) {
            delete node;
        }
    }

    void connect(CfgBlock<I>* block1, CfgBlock<I>* block2) {
        edges.set(block1->getIdx(), block2->getIdx());
    }

    void disconnect(CfgBlock<I>* block) {
        size_t idx = block->getIdx();
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
        return edges.get(block1->getIdx(), block2->getIdx());
    }

    bool hasConnections(const CfgBlock<I>* block) const {
        size_t idx = block->getIdx();
        size_t edgesCount = edges.getRowsCount();
        for (size_t i = 0; i < edgesCount; i++) {
            if (edges.get(idx, i) + edges.get(i, idx) > 0) {
                return true;
            }
        }
        return false;
    }

    SIterator successorsIterator(const CfgBlock<I>* block) const {
        return SIterator(blocks, edges, block->getIdx());
    }

    SIterator successorsIterator(size_t blockIdx) const {
        if (blockIdx >= blocks.size()) {
            sparkError("CfGraph", "Failed to get successors iterator: Invalid index %d", blockIdx);
        }
        return SIterator(blocks, edges, blockIdx);
    }

    SIterator sEnd() const {
        return SIterator(blocks, edges, blocks.size(), blocks.size());
    }

    PIterator precedessorsIterator(const CfgBlock<I>* block) const {
        return PIterator(blocks, edges, block->getIdx());
    }

    PIterator precedessorsIterator(size_t blockIdx) const {
        if (blockIdx >= blocks.size()) {
            sparkError("CfGraph", "Failed to get precedessors iterator: Invalid index %d", blockIdx);
        }
        return PIterator(blocks, edges, blockIdx);
    }

    PIterator pEnd() const {
        return PIterator(blocks, edges, blocks.size(), blocks.size());
    }

    CfgBlock<I>* getBlock(int index) const {
        return blocks[index];
    }

    const std::vector<CfgBlock<I>*>& getBlocks() const {
        return blocks;
    }

    size_t getBlocksCount() const {
        return blocks.size();
    }

    void toPlain(std::vector<I>& out) {
        for (CfgBlock<I>* block : blocks) {
            if (hasConnections(block)) {
                block->copyTo(out);
            }
        }
    }

    bool isEntryBlock(size_t blockIdx) const {
        return blockIdx == 0;
    }

    bool isEndBlock(size_t blockIdx) const {
        return blockIdx == blocks.size() - 1;
    }

private:
    std::vector<CfgBlock<I>*> blocks;
    BitMatrix edges;
};
