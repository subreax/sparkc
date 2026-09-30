#pragma once
#include <vector>
#include "sparkc/common/StringRef.h"
#include "sparkc/symbol/SymbolTable.h"
#include "sparkc/skr/optimizer/SkrCfg.h"

class SkrOptimizerUtils {
public:
    template <typename T>
    static void filterNullptrs(std::vector<T>& vec) {
        size_t offset = 0;
        for (size_t i = 0; i < vec.size(); i++) {
            if (vec[i] != nullptr) {
                vec[i - offset] = vec[i];
            }
            else {
                offset++;
            }
        }

        if (offset > 0) {
            vec.resize(vec.size() - offset);
        }
    }

    static std::vector<StringRef> getStaticVars(SymbolTable& symTable) {
        std::vector<StringRef> vars;
        for (const auto& [name, symbol] : symTable) {
            if (symbol.getType()->kind != SymbolType::Kind::Function && symbol.isStatic()) {
                vars.emplace_back(name);
            }
        }
        return vars;
    }

    static std::vector<StringRef> getAliasedVars(const SkrCfg& graph) {
        std::vector<StringRef> referencedVars;
        for (size_t i = 0; i < graph.getSize(); i++) {
            const auto& blockBody = graph[i].getBody();
            for (const auto* instr : blockBody) {
                if (instr->kind == SkrInstruction::Kind::GetAddr) {
                    const auto* it = (const SkrGetAddr*) instr;
                    referencedVars.emplace_back(it->getVar()->getId());
                }
            }
        }
        return referencedVars;
    }
};
