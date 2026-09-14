#pragma once
#include <unordered_map>
#include "sparkc/common/StringRef.h"
#include "sparkc/common/alloc/Allocator.h"
#include "sparkc/symbol/except/UndeclaredSymbolException.h"
#include "SymbolTypeFactory.h"
#include "Symbol.h"

class SymbolTable {
public:
    SymbolTable(Allocator& allocator)
        : typeFactory(allocator) { }

    void declareVar(StringRef name, SymbolType* type, bool isStatic);
    void redeclareVar(StringRef name, SymbolType* type, bool isStatic);
    void declareFunc(StringRef name, SymbolType* retType, const std::vector<SymbolType*>& params);

    Symbol& get(StringRef name) {
        auto it = table.find(name);
        if (it == table.end()) {
            throw UndeclaredSymbolException(name);
        }
        return it->second;
    }

    const Symbol& get(StringRef name) const {
        auto it = table.find(name);
        if (it == table.end()) {
            throw UndeclaredSymbolException(name);
        }
        return it->second;
    }

    SymbolTypeFactory& getTypeFactory() { return typeFactory; }

    std::unordered_map<StringRef, Symbol>::const_iterator begin() const {
        return table.begin();
    }

    std::unordered_map<StringRef, Symbol>::const_iterator end() const {
        return table.end();
    }

private:
    std::unordered_map<StringRef, Symbol> table;
    SymbolTypeFactory typeFactory;
};
