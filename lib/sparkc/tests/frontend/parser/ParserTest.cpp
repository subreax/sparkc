#include "utils/MatchTestLoader.h"
#include "sparkc/common/alloc/LinearAllocator.h"
#include "sparkc/frontend/Frontend.h"
#include "sparkc/frontend/utils/AstPrinter.h"
#include <catch2/catch_test_macros.hpp>
#include "utils/MatchTestRunner.h"
#include "TestOptions.h"

extern TestOptions testOptions;

static std::string buildAst(const std::string& src) {
    LinearAllocator allocator("parser", 4096, true);
    LinearAllocator commonAllocator("common", 2048, true);
    SymbolTable symbolTable(commonAllocator);
    TypeTable typeTable(commonAllocator);
    IdentifierGen idGen(commonAllocator);

    AstFactory astFactory(allocator);
    Frontend frontend(src.c_str(), astFactory, symbolTable, typeTable, idGen);
    AstProgram* program = frontend.processFullSource();

    std::ostringstream actualTree;
    AstPrinter(actualTree).print(program);
    return actualTree.str();
}

TEST_CASE("Parser Tests", "[parser]") {
    MatchTestRunner::runAllTests(testOptions.parserMatchTestsPath, buildAst);
}
