#include "AstTester.h"
#include "ParserTestLoader.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Parser Tests", "[parser]") {
    ParserTestLoader::loadRecursively("tests\\ast", [](const ParserTest& test) {
        testAst(test.path.c_str(), test.src.c_str(), test.expectedTree.c_str());
    });
}
