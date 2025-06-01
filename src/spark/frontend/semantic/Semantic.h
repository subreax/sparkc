#pragma once
#include "IdentifierResolution.h"
#include "TypeChecker.h"

class Semantic {
public:
    Semantic(SymbolTable& table, Allocator& typeAlloc, Allocator& astAlloc)
        : table(table)
        , typeAlloc(typeAlloc)
        , astAlloc(astAlloc) {  }

    void process(AstProgram* prog) {
        IdentifierResolution(table, typeAlloc).resolve(prog);
        TypeChecker(table, astAlloc).typeCheck(prog);
    }
    
private:
    SymbolTable& table;
    Allocator& typeAlloc;
    Allocator& astAlloc;
};
