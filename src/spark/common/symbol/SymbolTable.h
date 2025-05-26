#pragma once
#include <map>
#include "../LinearAllocator.h"
#include "../CStringLessThan.h"
#include "SymbolType.h"
#include "DuplicateSymbolDeclarationException.h"
#include "UndeclaredSymbolException.h"

class SymbolTable {
public:
    void declare(const char* name, SymbolType* type) {
        auto it = table.find(name);
        if (it != table.end()) {
            throw DuplicateSymbolDeclarationException(name, type);
        }

        table[name] = type;
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
