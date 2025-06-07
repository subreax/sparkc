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

    const std::vector<I>& getBody() const { return body; }
    int getId() const { return id; }

    bool isEmpty() { return body.empty(); }
    bool isNotEmpty() { return !isEmpty(); }

private:
    std::vector<I> body;
    const int id;
};