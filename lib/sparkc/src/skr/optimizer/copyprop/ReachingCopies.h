#pragma once
#include "sparkc/skr/instr/SkrCopy.h"
#include <vector>

class ReachingCopies {
public:
    ReachingCopies() = default;
    ReachingCopies(const std::vector<SkrCopy*>& initial)
        : copies(initial) { }

    void add(SkrCopy* copy) {
        if (!contains(copy)) {
            copies.emplace_back(copy);
        }
    }

    void kill(const SkrVar* var) {
        for (size_t i = 0; i < copies.size(); i++) {
            auto* copy = copies[i];
            if (*var == *copy->getFrom() || *var == *copy->getTo()) {
                copies.erase(copies.begin() + i);
                i--;
            }
        }
    }

    bool contains(const SkrValue* dst, const SkrValue* src) const {
        for (auto* copy : copies) {
            if (*dst == *copy->getTo() && *src == *copy->getFrom()) {
                return true;
            }
        }
        return false;
    }

    bool contains(const SkrCopy* copy) const {
        return contains(copy->getTo(), copy->getFrom());
    }

    SkrCopy* findByDst(const SkrValue* dst) const {
        for (auto* copy : copies) {
            if (*dst == *copy->getTo()) {
                return copy;
            }
        }
        return nullptr;
    }

    void clear() {
        copies.clear();
    }

    void reserve(size_t n) {
        copies.reserve(n);
    }

    void intersect(const ReachingCopies& other) {
        for (size_t i = 0; i < copies.size(); i++) {
            if (!other.contains(copies[i])) {
                copies.erase(copies.begin() + i);
                i--;
            }
        }
    }

    const std::vector<SkrCopy*>& getCopies() const {
        return copies;
    }

    size_t size() const {
        return copies.size();
    }

    bool operator==(const ReachingCopies& other) const {
        if (copies.size() != other.copies.size()) {
            return false;
        }

        for (size_t i = 0; i < copies.size(); i++) {
            if (!contains(other.copies[i])) {
                return false;
            }
        }

        return true;
    }

    bool operator!=(const ReachingCopies& other) const {
        return !(*this == other);
    }

private:
    std::vector<SkrCopy*> copies;
};