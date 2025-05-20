#pragma once
#include <cstdint>
#include <vector>

class RvListing {
public:
    RvListing(std::vector<uint32_t>& out) : outBin(out) {  }

    void add(uint32_t instr) {
        outBin.emplace_back(instr);
        pc += 4;
    }

    RvListing& operator+=(uint32_t instr) {
        add(instr);
        return *this;
    }

    void addLabel(const char* label) {
        labels.emplace_back(pc, label);
    }

    void addWithLabel(uint32_t instr, const char* label) {
        outBin.emplace_back(instr);
        unresolved.emplace_back(pc, label);
        pc += 4;
    }

private:
    struct IdxAndLabel {
        IdxAndLabel() = default;
        IdxAndLabel(int idx, const char* label) : idx(idx), label(label) {  }

        int idx;
        const char* label;
    };

    int pc;
    std::vector<uint32_t>& outBin;
    std::vector<IdxAndLabel> labels;
    std::vector<IdxAndLabel> unresolved;
};