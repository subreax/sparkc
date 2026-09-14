#pragma once
#include "sparkc/frontend/ast/everything.h"
#include "sparkc/frontend/parser/Parser.h"
#include "sparkc/frontend/ast/AstFactory.h"
#include "sparkc/symbol/SymbolTable.h"
#include "sparkc/type/TypeTable.h"
#include "sparkc/frontend/semantic/Semantic.h"
#include "sparkc/common/IdentifierGen.h"
#include "sparkc/SparkStageCallback.h"

class Frontend {
public:
    Frontend(const char* src, AstFactory& astFactory, SymbolTable& symTable, TypeTable& typeTable, IdentifierGen& idGen, SparkStageCallback& stageCallback)
        : astFactory(astFactory)
        , lexer(src)
        , parser(lexer, astFactory, symTable.getTypeFactory())
        , semantic(astFactory, symTable, typeTable, idGen)
        , symTable(symTable)
        , typeTable(typeTable)
        , idGen(idGen)
        , stageCallback(stageCallback) { }

    bool hasNext() const {
        return parser.hasNext();
    }

    AstProgItem* processNextItem() {
        auto* item = parser.parseNextProgItem();
        semantic.process(item);
        stageCallback.onAstBuild(item);
        return item;
    }

    AstProgram* processFullSource() {
        std::vector<AstProgItem*> items;
        while (hasNext()) {
            items.emplace_back(processNextItem());
        }
        return astFactory.program(items);
    }

private:
    AstFactory& astFactory;
    Lexer lexer;
    Parser parser;
    Semantic semantic;
    SymbolTable& symTable;
    TypeTable& typeTable;
    IdentifierGen& idGen;
    SparkStageCallback& stageCallback;
};