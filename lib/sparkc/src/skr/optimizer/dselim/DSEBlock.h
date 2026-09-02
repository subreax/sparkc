#pragma once
#include "VarSet.h"
#include <algorithm>

class DSEBlock {
public:
    void resize(size_t newSize) {
        instrVars.resize(newSize);
    }

    void setBlockVars(const VarSet& vars) {
        blockVars = vars;
    }

    const std::vector<VarSet>& getInstrVars() const {
        return instrVars;
    }

    const VarSet& getBlockVars() const {
        return blockVars;
    }

    void clear() {
        instrVars.clear();
        blockVars.clear();
    }

    VarSet& operator[](size_t idx) {
        return instrVars[idx];
    }

    const VarSet& operator[](size_t idx) const {
        return instrVars[idx];
    }

    bool operator==(const DSEBlock& other) {
        return blockVars == other.blockVars && instrVars == other.instrVars;
    }

    bool operator!=(const DSEBlock& other) {
        return !(*this == other);
    }

private:
    std::vector<VarSet> instrVars;
    VarSet blockVars;
};