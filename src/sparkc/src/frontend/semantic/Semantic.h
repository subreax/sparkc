#pragma once
#include "IdentifierResolution.h"
#include "TypeChecker.h"

class Semantic {
public:
    Semantic(SymbolTable& symbolTable, TypeTable& typeTable, IdentifierGen& idGen, Allocator& astAlloc, size_t scopeMem)
        : symbolTable(symbolTable)
        , typeTable(typeTable)
        , idGen(idGen)
        , astAlloc(astAlloc)
        , scopeMem(scopeMem) {  }

    void process(AstProgram* prog) {
        IdentifierResolution(symbolTable, typeTable, idGen, scopeMem).resolve(prog);
        TypeChecker(symbolTable, typeTable, astAlloc).typeCheck(prog);
    }
    
private:
    SymbolTable& symbolTable;
    TypeTable& typeTable;
    IdentifierGen& idGen;
    Allocator& astAlloc;
    size_t scopeMem;
};
