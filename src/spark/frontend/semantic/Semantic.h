#pragma once
#include "IdentifierResolution.h"
#include "TypeChecker.h"

class Semantic {
public:
    Semantic(SymbolTable& table, Allocator& astAlloc)
        : table(table)
        , astAlloc(astAlloc) {  }

    void process(AstProgram* prog) {
        IdentifierResolution(table).resolve(prog);
        TypeChecker(table, astAlloc).typeCheck(prog);
    }
    
private:
    SymbolTable& table;
    Allocator& astAlloc;
};
