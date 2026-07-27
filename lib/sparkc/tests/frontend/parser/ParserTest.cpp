#include "AstTester.h"
#include "ParserTestLoader.h"
#include <catch2/catch_test_macros.hpp>
#include "TestOptions.h"

extern TestOptions testOptions;

TEST_CASE("Parser Tests", "[parser]") {
    ParserTestLoader::loadRecursively(testOptions.parserMatchTestsPath, [](const ParserTest& test) {
        testAst(test);
    });
}
