#pragma once
#include "sparkc/skr/instr/SkrCopy.h"
#include <vector>

class ReachingCopiesUtils {
public:
    static bool contains(const std::vector<SkrCopy*>& vec, SkrCopy* copy) {
        for (const auto* copy1 : vec) {
            if (*copy == *copy1) {
                return true;
            }
        }
        return false;
    }

    static void intersect(std::vector<SkrCopy*>& target, const std::vector<SkrCopy*>& vec) {
        for (size_t i = 0; i < target.size(); i++) {
            if (!contains(vec, target[i])) {
                target.erase(target.begin() + i);
                i--;
            }
        }
    }
};