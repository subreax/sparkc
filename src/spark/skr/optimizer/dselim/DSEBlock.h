#pragma once
#include "VarSet.h"
#include <algorithm>

class DSEBlock {
public:
    void addInstrVars(const VarSet& vars) {
        instrVars.emplace_back(vars);
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

    void reverseInstructions() {
        std::reverse(instrVars.begin(), instrVars.end());
    }

    void clear() {
        instrVars.clear();
        blockVars.clear();
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