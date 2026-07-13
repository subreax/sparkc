#include "AstTester.h"
#include "ParserTestLoader.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Parser Tests", "[parser]") {
    ParserTestLoader::loadRecursively("match-tests\\ast", [](const ParserTest& test) {
        testAst(test);
    });
}
