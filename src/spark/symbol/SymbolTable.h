#pragma once
#include <unordered_map>
#include "../common/StringRef.h"
#include "../common/alloc/Allocator.h"
#include "SymbolType.h"
#include "except/DuplicateSymbolDeclarationException.h"
#include "except/UndeclaredSymbolException.h"

class SymbolTable {
public:
    SymbolTable(Allocator& typeAllocator) : typeAllocator(typeAllocator) {  }

    void declare(const char* name, SymbolType* type) {
        StringRef name1(name, StringRef::lengthOf(name));
        auto it = table.find(name1);
        if (it != table.end()) {
            throw DuplicateSymbolDeclarationException(name, type);
        }

        table.emplace(name1, type);
    }

    SymbolType* get(const char* name) const {
        StringRef name1(name, StringRef::lengthOf(name));
        auto it = table.find(name1);
        if (it == table.end()) {
            throw UndeclaredSymbolException(name);
        }
        return it->second;
    }

    Allocator& getTypeAllocator() const { return typeAllocator; }

private:
    std::unordered_map<StringRef, SymbolType*> table;
    Allocator& typeAllocator;
};
