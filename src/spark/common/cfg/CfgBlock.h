#pragma once
#include <vector>

namespace cfg {
template<typename I>
bool isLabel(I* ptr);

// return empty string if instruction doesn't have labels or jumps
template<typename I>
const char* getLabel(I* ptr);

template<typename I>
bool isJump(I* ptr);

template<typename I>
bool isBranch(I* ptr);
};



template<typename I>
class CfgBlock {
public:
    CfgBlock(int id) : id(id) {  }

    CfgBlock(int id, const std::vector<I>& body) 
        : id(id)
        , body(body) {  }

    void add(const I& instr) {
        body.emplace_back(instr);
    }

    std::vector<I>& getBody() { return body; }
    const std::vector<I>& getBody() const { return body; }

    int getId() const { return id; }

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

    const char* getLabel() const {
        return cfg::getLabel(body.front());
    }

    const char* getJumpOrBranchLabel() const {
        return cfg::getLabel(body.back());
    }

private:
    std::vector<I> body;
    const int id;
};