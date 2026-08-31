#pragma once
#include "../StringRef.h"
#include <vector>

namespace cfg {
template <typename I>
bool isLabel(const I& instr);

// return empty string if instruction doesn't have labels or jumps
template <typename I>
StringRef getLabel(const I& instr);

template <typename I>
bool isJump(const I& instr);

template <typename I>
bool isBranch(const I& instr);
}; // namespace cfg

template <typename I>
class CfgBlock {
public:
    CfgBlock() = default;

    CfgBlock(size_t idx)
        : idx(idx) { }

    CfgBlock(size_t idx, const std::vector<I>& body)
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

    size_t getIdx() const { return idx; }

    bool isEmpty() const { return body.empty(); }
    bool isNotEmpty() const { return !isEmpty(); }

    void copyTo(std::vector<I>& out) const {
        for (const I& instr : body) {
            out.emplace_back(instr);
        }
    }

    void eraseLastInstruction() {
        body.erase(body.end() - 1);
    }

    void eraseFirstInstruction() {
        body.erase(body.begin());
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
    const size_t idx = -1;
};