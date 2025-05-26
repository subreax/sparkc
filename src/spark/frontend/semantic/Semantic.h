#pragma once
#include "IdentifierResolution.h"
#include "TypeChecker.h"

class Semantic {
public:
    Semantic(SymbolTable& table, LinearAllocator& typeAlloc)
        : table(table)
        , typeAlloc(typeAlloc) {  }

    void process(AstProgram* prog) {
        IdentifierResolution(table, typeAlloc).resolve(prog);
        TypeChecker(table).typeCheck(prog);
    }
    
private:
    SymbolTable& table;
    LinearAllocator& typeAlloc;
};
