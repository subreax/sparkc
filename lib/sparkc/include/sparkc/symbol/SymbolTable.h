#pragma once
#include "sparkc/common/StringRef.h"
#include "sparkc/common/alloc/Allocator.h"
#include "sparkc/symbol/except/UndeclaredSymbolException.h"
#include "sparkc/symbol/except/UndeclaredSymbolException.h"
#include "SymbolType.h"
#include "SymbolTypeFactory.h"
#include <unordered_map>

class SymbolTable {
public:
    SymbolTable(Allocator& allocator)
        : typeFactory(allocator) { }

    void declareVar(StringRef name, SymbolType* type);
    void redeclareVar(StringRef name, SymbolType* type);
    void declareFunc(StringRef name, SymbolType* retType, const std::vector<SymbolType*>& params);

    SymbolType* get(StringRef name) const {
        auto it = table.find(name);
        if (it == table.end()) {
            throw UndeclaredSymbolException(name);
        }
        return it->second;
    }

    SymbolTypeFactory& getTypeFactory() { return typeFactory; }

    std::unordered_map<StringRef, SymbolType*>::const_iterator begin() const {
        return table.begin();
    }

    std::unordered_map<StringRef, SymbolType*>::const_iterator end() const {
        return table.end();
    }

private:
    std::unordered_map<StringRef, SymbolType*> table;
    SymbolTypeFactory typeFactory;
};
