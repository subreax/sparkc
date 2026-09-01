#pragma once
#include "ReachingCopies.h"
#include "sparkc/skr/instr/SkrCopy.h"

class RCABlock {
public:
    void addInstructionAnnotation(const ReachingCopies& rc) {
        instructionAnnotations.emplace_back(rc.getCopies());
    }

    void setBlockAnnotation(const ReachingCopies& rc) {
        blockAnnotation = rc.getCopies();
    }

    void clear() {
        instructionAnnotations.clear();
        blockAnnotation.clear();
    }

    bool operator==(const RCABlock& other) const {
        if (blockAnnotation != other.blockAnnotation) {
            return false;
        }

        if (instructionAnnotations.size() != other.instructionAnnotations.size()) {
            return false;
        }

        for (size_t i = 0; i < instructionAnnotations.size(); i++) {
            if (instructionAnnotations[i] != other.instructionAnnotations[i]) {
                return false;
            }
        }

        return true;
    }

    bool operator!=(const RCABlock& other) {
        return !(*this == other);
    }

    std::vector<ReachingCopies> instructionAnnotations;
    ReachingCopies blockAnnotation;
};