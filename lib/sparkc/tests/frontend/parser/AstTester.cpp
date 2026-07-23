#include "AstTester.h"
#include "frontend/parser/Parser.h"
#include "frontend/semantic/Semantic.h"
#include "sparkc/common/alloc/LinearAllocator.h"
#include "sparkc/frontend/ast/printer/AstPrinter.h"
#include "sparkc/type/TypeTable.h"
#include <catch2/catch_test_macros.hpp>

std::string buildAst(const char* src) {
    LinearAllocator allocator("parser", 2048, true);
    LinearAllocator commonAllocator("common", 2048, true);
    SymbolTable symbolTable(commonAllocator);
    TypeTable typeTable(commonAllocator);
    IdentifierGen idGen(commonAllocator);

    Lexer lexer(src);
    Parser parser(lexer, allocator, commonAllocator);

    AstProgram* program = parser.parseProgram();
    Semantic(symbolTable, typeTable, idGen, allocator).process(program);

    std::ostringstream actualTree;
    AstPrinter(actualTree).print(program);
    return actualTree.str();
}

void testAst(const ParserTest& test) {
    INFO("Test: " << test.path);
    bool expectAst = !test.expectFailure;
    if (expectAst) {
        try {
            auto actualTree = buildAst(test.src.c_str());
            REQUIRE(actualTree == test.expectedTree);
        } catch (std::exception& ex) {
            FAIL("Failed with exception:\n  " << ex.what());
        }
    }
    else {
        REQUIRE_THROWS(buildAst(test.src.c_str()));
    }
}
