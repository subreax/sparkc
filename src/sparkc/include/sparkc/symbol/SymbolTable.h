#pragma once
#include <unordered_map>
#include "../common/StringRef.h"
#include "../common/alloc/Allocator.h"
#include "SymbolType.h"
#include "except/DuplicateSymbolDeclarationException.h"
#include "except/UndeclaredSymbolException.h"

class SymbolTable {
public:
    SymbolTable(Allocator& allocator) : allocator(allocator) {  }

    void declare(StringRef name, SymbolType* type) {
        auto it = table.find(name);
        if (it != table.end()) {
            throw DuplicateSymbolDeclarationException(name, type);
        }

        table.emplace(name, type);
    }

    void declareFunc(StringRef name, SymbolType* retType, const std::vector<SymbolType*>& params) {
        BoundArray<SymbolType*> paramTypes = BoundArray<SymbolType*>::fromVector(params, allocator);
        auto* type = allocator.create<SymbolFunctionType>(retType, paramTypes);

        auto it = table.find(name);
        if (it != table.end()) {
            throw DuplicateSymbolDeclarationException(name, type);
        }

        table.emplace(name, type);
    }

    SymbolType* get(StringRef name) const {
        auto it = table.find(name);
        if (it == table.end()) {
            throw UndeclaredSymbolException(name);
        }
        return it->second;
    }

    Allocator& getTypeAllocator() const { return allocator; }

    std::unordered_map<StringRef, SymbolType*>::const_iterator begin() const {
        return table.begin();
    }

    std::unordered_map<StringRef, SymbolType*>::const_iterator end() const {
        return table.end();
    }

private:
    std::unordered_map<StringRef, SymbolType*> table;
    Allocator& allocator;
};
