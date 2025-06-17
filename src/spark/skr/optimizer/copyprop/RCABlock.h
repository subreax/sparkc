#pragma once
#include "../../instr/everything.h"
#include "ReachingCopies.h"
#include "ReachingCopiesUtils.h"

struct RCABlock {
    RCABlock() = default;

    void addInstrCopies(const ReachingCopies& rc) {
        instrCopies.emplace_back(rc.getCopies());
    }

    void clear() {
        instrCopies.clear();
        blockCopies.clear();
    }

    bool operator==(const RCABlock& other) const {
        if (!areCopiesEqual(blockCopies, other.blockCopies)) {
            return false;
        }

        if (instrCopies.size() != other.instrCopies.size()) {
            return false;
        }

        for (size_t i = 0; i < instrCopies.size(); i++) {
            if (!areCopiesEqual(instrCopies[i], other.instrCopies[i])) {
                return false;
            }
        }

        return true;
    }

    bool operator!=(const RCABlock& other) {
        return !(*this == other);
    }

    std::vector<std::vector<SkrCopy*>> instrCopies;
    std::vector<SkrCopy*> blockCopies;

private:
    static bool areCopiesEqual(const std::vector<SkrCopy*>& v1, const std::vector<SkrCopy*>& v2) {
        if (v1.size() != v2.size()) {
            return false;
        }

        for (size_t i = 0; i < v2.size(); i++) {
            if (!ReachingCopiesUtils::contains(v1, v2[i])) {
                return false;
            }
        }

        return true;
    }
};