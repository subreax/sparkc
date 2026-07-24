#include "AstTester.h"
#include <catch2/catch_test_macros.hpp>
#include "sparkc/frontend/parser/Parser.h"
#include "sparkc/frontend/semantic/Semantic.h"
#include "sparkc/common/alloc/LinearAllocator.h"
#include "sparkc/frontend/utils/AstPrinter.h"
#include "sparkc/type/TypeTable.h"

std::string buildAst(const char* src) {
    LinearAllocator allocator("parser", 4096, true);
    LinearAllocator commonAllocator("common", 2048, true);
    SymbolTable symbolTable(commonAllocator);
    TypeTable typeTable(commonAllocator);
    IdentifierGen idGen(commonAllocator);

    Lexer lexer(src);
    AstFactory astFactory(allocator);
    Parser parser(lexer, astFactory, symbolTable.getTypeFactory());

    AstProgram* program = parser.parseProgram();
    Semantic(astFactory, symbolTable, typeTable, idGen).process(program);

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
