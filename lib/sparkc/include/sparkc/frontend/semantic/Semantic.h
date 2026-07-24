#pragma once
#include "IdentifierResolution.h"
#include "TypeChecker.h"

class Semantic {
public:
    Semantic(
        AstFactory& astFactory,
        SymbolTable& symbolTable,
        TypeTable& typeTable,
        IdentifierGen& idGen
    )
        : symbolTable(symbolTable)
        , typeTable(typeTable)
        , idGen(idGen)
        , astFactory(astFactory) { }

    void process(AstProgram* prog) {
        IdentifierResolution(astFactory, symbolTable, typeTable, idGen).resolve(prog);
        TypeChecker(astFactory, symbolTable, typeTable).typeCheck(prog);
    }

private:
    AstFactory& astFactory;
    SymbolTable& symbolTable;
    TypeTable& typeTable;
    IdentifierGen& idGen;
};
