#pragma once
#include "../symbol/SymbolTable.h"
#include "../type/TypeTable.h"

class SymbolSize {
public:
    SymbolSize(SymbolTable& symbolTable, TypeTable& typeTable)
        : symbolTable(symbolTable), typeTable(typeTable) {  }

    size_t get(StringRef id) {
        return get(symbolTable.get(id));
    }

    size_t get(SymbolType* type) {
        if (type->kind != SymbolType::Kind::Structure) {
            return 4;
        }

        auto* structType = (SymbolStructureType*) type;
        return typeTable.getStructSize(structType->getTag());
    }

private:
    SymbolTable& symbolTable;
    TypeTable& typeTable;
};