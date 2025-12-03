#pragma once
#include <vector>
#include "sparkc/skr/value/SkrValue.h"

class VarSet {
public:
    void generate(const SkrVar* var) {
        auto it = find(var);
        if (it == vars.end()) {
            vars.emplace_back(var);
        }
    }

    void generateIfVar(const SkrValue* value) {
        if (value->isVar()) {
            generate(value->toSkrVar());
        }
    }

    void kill(const SkrVar* var) {
        auto it = find(var);
        if (it != vars.end()) {
            vars.erase(it);
        }
    }

    const std::vector<const SkrVar*>& getVars() const {
        return vars;
    }

    void addAll(const VarSet& other) {
        for (auto* var : other.vars) {
            generate(var);
        }
    }

    void clear() {
        vars.clear();
    }

    bool operator==(const VarSet& other) const {
        if (vars.size() != other.vars.size()) {
            return false;
        }

        for (size_t i = 0; i < vars.size(); i++) {
            if (*vars[i] != *other.vars[i]) {
                return false;
            }
        }
        return true;
    }

    bool operator!=(const VarSet& other) const {
        return !(*this == other);
    }

    bool contains(const SkrVar* var) const {
        return find(var) != vars.end();
    }

    bool contains(const SkrValue* value) const {
        if (value->isVar()) {
            return contains((const SkrVar*) value);
        }
        return false;
    }

private:
    std::vector<const SkrVar*>::const_iterator find(const SkrVar* var) const {
        auto it = vars.begin();
        auto end = vars.end();
        while (it != end) {
            if (**it == *var) {
                break;
            }
            ++it;
        }
        return it;
    }


    std::vector<const SkrVar*> vars;
};