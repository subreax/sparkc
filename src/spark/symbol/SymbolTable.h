#pragma once
#include <map>
#include "../common/LinearAllocator.h"
#include "../common/CStringLessThan.h"
#include "SymbolType.h"
#include "except/DuplicateSymbolDeclarationException.h"
#include "except/UndeclaredSymbolException.h"

class SymbolTable {
public:
    void declare(const char* name, SymbolType* type) {
        auto it = table.find(name);
        if (it != table.end()) {
            throw DuplicateSymbolDeclarationException(name, type);
        }

        table.emplace(name, type);
    }

    SymbolType* get(const char* name) const {
        auto it = table.find(name);
        if (it == table.end()) {
            throw UndeclaredSymbolException(name);
        }
        return it->second;
    }

private:
    std::map<const char*, SymbolType*, CStringLessThan> table;
};
