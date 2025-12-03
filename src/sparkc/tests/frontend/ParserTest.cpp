#include <catch2/catch_test_macros.hpp>
#include "spark/frontend/parser/Parser.h"
#include "spark/common/alloc/LinearAllocator.h"
#include "standalone/printer/ast/AstPrinter.h"

TEST_CASE("Test Parser", "[parser]") {
    LinearAllocator allocator("parser", 2048, true);
    LinearAllocator typeAllocator("type", 512, true);
    TypeTable typeTable(typeAllocator);

    SECTION("Parse base function") {
        const char* src = 
R"~(int main() {
    return 0;
}
)~";

        const char* expectedTree = 
R"~(AstProgram(
  items: [
    AstFunction(
      name: 'main',
      returnType: 'int',
      params: [],
      block: AstBlock(
        items: [
          AstReturnStatement(
            return: AstConstantExp(
              type: 'int',
              value: '0'
            )
          )
        ]
      )
    )
  ]
))~";

        Lexer lexer(src);

        std::vector<StringRef> types;
        Parser parser(lexer, allocator, typeTable, types);

        AstProgram* program = parser.parseProgram();
        std::ostringstream actualTree;
        AstPrinter(actualTree).print(program);
        REQUIRE(actualTree.str() == expectedTree);
    }
}
