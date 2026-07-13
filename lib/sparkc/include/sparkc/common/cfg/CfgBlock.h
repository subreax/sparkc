#pragma once
#include "../StringRef.h"
#include <vector>

namespace cfg {
template <typename I>
bool isLabel(I* ptr);

// return empty string if instruction doesn't have labels or jumps
template <typename I>
StringRef getLabel(I* ptr);

template <typename I>
bool isJump(I* ptr);

template <typename I>
bool isBranch(I* ptr);
}; // namespace cfg

template <typename I>
class CfgBlock {
public:
    CfgBlock(int idx)
        : idx(idx) { }

    CfgBlock(int idx, const std::vector<I>& body)
        : idx(idx)
        , body(body) { }

    void add(const I& instr) {
        body.emplace_back(instr);
    }

    std::vector<I>& getBody() { return body; }
    const std::vector<I>& getBody() const { return body; }

    I& get(size_t idx) {
        return body[idx];
    }

    const I& get(size_t idx) const {
        return body[idx];
    }

    int getIdx() const { return idx; }

    bool isEmpty() { return body.empty(); }
    bool isNotEmpty() { return !isEmpty(); }

    void copyTo(std::vector<I>& out) {
        for (I& instr : body) {
            out.emplace_back(instr);
        }
    }

    bool isLabeled() const {
        return !body.empty() && cfg::isLabel(body.front());
    }

    bool hasJump() const {
        return !body.empty() && cfg::isJump(body.back());
    }

    bool hasBranch() const {
        return !body.empty() && cfg::isBranch(body.back());
    }

    StringRef getLabel() const {
        return cfg::getLabel(body.front());
    }

    StringRef getJumpOrBranchLabel() const {
        return cfg::getLabel(body.back());
    }

    I& operator[](size_t idx) {
        return get(idx);
    }

    const I& operator[](size_t idx) const {
        return get(idx);
    }

private:
    std::vector<I> body;
    const int idx;
};