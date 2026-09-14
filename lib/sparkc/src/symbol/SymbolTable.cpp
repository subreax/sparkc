#include "sparkc/symbol/SymbolTable.h"
#include "sparkc/symbol/except/DuplicateSymbolDeclarationException.h"
#include <unordered_map>

void SymbolTable::declareVar(StringRef name, SymbolType* type, bool isStatic) {
    auto it = table.find(name);
    if (it != table.end()) {
        throw DuplicateSymbolDeclarationException(name, type);
    }

    table.emplace(name, Symbol(type, isStatic));
}

void SymbolTable::redeclareVar(StringRef name, SymbolType* type, bool isStatic) {
    table.insert_or_assign(name, Symbol(type, isStatic));
}

void SymbolTable::declareFunc(StringRef name, SymbolType* retType, const std::vector<SymbolType*>& params) {
    auto* type = typeFactory.function(retType, params);

    auto it = table.find(name);
    if (it != table.end()) {
        throw DuplicateSymbolDeclarationException(name, type);
    }

    table.emplace(name, Symbol(type, true));
}
