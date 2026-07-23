#pragma once
#include "IdentifierResolution.h"
#include "TypeChecker.h"

class Semantic {
public:
    Semantic(
        SymbolTable& symbolTable,
        TypeTable& typeTable,
        IdentifierGen& idGen,
        Allocator& astAlloc
    )
        : symbolTable(symbolTable)
        , typeTable(typeTable)
        , idGen(idGen)
        , astAlloc(astAlloc) { }

    void process(AstProgram* prog) {
        IdentifierResolution(astAlloc, symbolTable, typeTable, idGen).resolve(prog);
        TypeChecker(symbolTable, typeTable, astAlloc).typeCheck(prog);
    }

private:
    SymbolTable& symbolTable;
    TypeTable& typeTable;
    IdentifierGen& idGen;
    Allocator& astAlloc;
};
