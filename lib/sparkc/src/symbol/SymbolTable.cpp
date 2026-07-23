#include "sparkc/symbol/SymbolTable.h"
#include "sparkc/symbol/except/DuplicateSymbolDeclarationException.h"
#include <unordered_map>

void SymbolTable::declareVar(StringRef name, SymbolType* type) {
    auto it = table.find(name);
    if (it != table.end()) {
        throw DuplicateSymbolDeclarationException(name, type);
    }

    table.emplace(name, type);
}

void SymbolTable::redeclareVar(StringRef name, SymbolType* type) {
    table.insert_or_assign(name, type);
}

void SymbolTable::declareFunc(StringRef name, SymbolType* retType, const std::vector<SymbolType*>& params) {
    BoundArray<SymbolType*> paramTypes = BoundArray<SymbolType*>::fromVector(params, allocator);
    auto* type = allocator.create<SymbolFunctionType>(retType, paramTypes);

    auto it = table.find(name);
    if (it != table.end()) {
        throw DuplicateSymbolDeclarationException(name, type);
    }

    table.emplace(name, type);
}
