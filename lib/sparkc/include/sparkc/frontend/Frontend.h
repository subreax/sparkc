#pragma once
#include "ast/everything.h"
#include "sparkc/frontend/parser/Parser.h"
#include "sparkc/frontend/ast/AstFactory.h"
#include "sparkc/symbol/SymbolTable.h"
#include "sparkc/type/TypeTable.h"
#include "sparkc/frontend/semantic/Semantic.h"
#include "sparkc/common/IdentifierGen.h"

class Frontend {
public:
    Frontend(const char* src, AstFactory& astFactory, SymbolTable& symTable, TypeTable& typeTable, IdentifierGen& idGen)
        : astFactory(astFactory)
        , lexer(src)
        , parser(lexer, astFactory, symTable.getTypeFactory())
        , semantic(astFactory, symTable, typeTable, idGen)
        , symTable(symTable)
        , typeTable(typeTable)
        , idGen(idGen) { }

    bool hasNext() const {
        return parser.hasNext();
    }

    AstProgItem* processNextItem();
    AstProgram* processFullSource();

private:
    AstFactory& astFactory;
    Lexer lexer;
    Parser parser;
    Semantic semantic;
    SymbolTable& symTable;
    TypeTable& typeTable;
    IdentifierGen& idGen;
};