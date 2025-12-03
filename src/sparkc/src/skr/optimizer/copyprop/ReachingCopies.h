#pragma once
#include <vector>
#include "sparkc/skr/instr/SkrCopy.h"
#include "ReachingCopiesUtils.h"

class ReachingCopies {
public:
    ReachingCopies() = default;
    ReachingCopies(const std::vector<SkrCopy*>& initial) : copies(initial) {  }

    void add(SkrCopy* copy) {
        if (!ReachingCopiesUtils::contains(copies, copy)) {
            copies.emplace_back(copy);
        }
    }

    void kill(const SkrVar* var) {
        for (size_t i = 0; i < copies.size(); i++) {
            auto* copy = copies[i];
            if (*var == *copy->getFrom() || *var == *copy->getTo()) {
                erase(&i);
            }
        }
    }

    const std::vector<SkrCopy*>& getCopies() const {
        return copies;
    }

private:
    void erase(size_t* idx) {
        for (size_t i = *idx; i < copies.size() - 1; i++) {
            copies[i] = copies[i + 1];
        }
        copies.resize(copies.size() - 1);
        *idx--;
    }

    std::vector<SkrCopy*> copies;
};