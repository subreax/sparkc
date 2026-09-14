#pragma once
#include "sparkc/skr/value/SkrValue.h"
#include <vector>

class VarSet {
public:
    void generate(const SkrVar* var) {
        generate(var->getId());
    }

    void generateIfVar(const SkrValue* value) {
        if (value->isVar()) {
            generate(value->toSkrVar()->getId());
        }
    }

    void generate(StringRef id) {
        auto it = find(id);
        if (it == varIds.end()) {
            varIds.emplace_back(id);
        }
    }

    void kill(const SkrVar* var) {
        auto it = find(var);
        if (it != varIds.end()) {
            varIds.erase(it);
        }
    }

    const std::vector<StringRef>& getVars() const {
        return varIds;
    }

    void addAll(const VarSet& other) {
        for (auto var : other.varIds) {
            generate(var);
        }
    }

    void clear() {
        varIds.clear();
    }

    bool operator==(const VarSet& other) const {
        if (varIds.size() != other.varIds.size()) {
            return false;
        }

        for (size_t i = 0; i < varIds.size(); i++) {
            if (varIds[i] != other.varIds[i]) {
                return false;
            }
        }
        return true;
    }

    bool operator!=(const VarSet& other) const {
        return !(*this == other);
    }

    bool contains(const SkrVar* var) const {
        return find(var->getId()) != varIds.end();
    }

    bool contains(const SkrValue* value) const {
        if (value->isVar()) {
            return contains((const SkrVar*) value);
        }
        return false;
    }

private:
    std::vector<StringRef>::const_iterator find(StringRef id) const {
        auto it = varIds.begin();
        auto end = varIds.end();
        while (it != end) {
            if (*it == id) {
                break;
            }
            ++it;
        }
        return it;
    }

    std::vector<StringRef>::const_iterator find(const SkrVar* var) const {
        return find(var->getId());
    }

    std::vector<StringRef> varIds;
};