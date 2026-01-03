#include "AstTester.h"
#include "frontend/parser/Parser.h"
#include "frontend/semantic/Semantic.h"
#include "sparkc/common/alloc/LinearAllocator.h"
#include "sparkc/frontend/ast/printer/AstPrinter.h"
#include "sparkc/type/TypeTable.h"
#include <catch2/catch_test_macros.hpp>

void testAst(const char* tag, const char* src, const char* expectedTree) {
    LinearAllocator allocator("parser", 2048, true);
    LinearAllocator commonAllocator("common", 2048, true);
    SymbolTable symbolTable(commonAllocator);
    TypeTable typeTable(commonAllocator);
    IdentifierGen idGen(commonAllocator);

    Lexer lexer(src);
    Parser parser(lexer, allocator, commonAllocator);

    AstProgram* program = parser.parseProgram();
    Semantic(symbolTable, typeTable, idGen, allocator, 2048).process(program);

    std::ostringstream actualTree;
    AstPrinter(actualTree).print(program);
    INFO("Failed test: " << tag);
    REQUIRE(actualTree.str() == expectedTree);
}
