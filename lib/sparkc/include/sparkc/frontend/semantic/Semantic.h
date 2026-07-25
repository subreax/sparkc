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
        : identifierResolution(astFactory, symbolTable, typeTable, idGen)
        , typeChecker(astFactory, symbolTable, typeTable) { }

    void process(AstProgItem* item) {
        identifierResolution.resolve(item);
        typeChecker.typeCheck(item);
    }

private:
    IdentifierResolution identifierResolution;
    TypeChecker typeChecker;
};
