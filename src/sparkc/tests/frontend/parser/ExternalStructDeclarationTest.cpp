#include "ParserTestLoader.h"
#include "frontend/parser/Parser.h"
#include "frontend/semantic/Semantic.h"
#include "sparkc/common/alloc/LinearAllocator.h"
#include "sparkc/frontend/ast/printer/AstPrinter.h"
#include "sparkc/type/TypeTable.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("External struct declaration test", "[parser]") {
    LinearAllocator allocator("parser", 2048, true);
    LinearAllocator commonAllocator("common", 2048, true);
    SymbolTable symbolTable(commonAllocator);
    TypeTable typeTable(commonAllocator);

    typeTable.declare(
        StringRef::cstr("color"),
        { StructField(StringRef::cstr("r"), SymbolFloatType::getInstance()),
          StructField(StringRef::cstr("g"), SymbolFloatType::getInstance()),
          StructField(StringRef::cstr("b"), SymbolFloatType::getInstance()),
          StructField(StringRef::cstr("a"), SymbolFloatType::getInstance()) }
    );

    IdentifierGen idGen(commonAllocator);

    const char* src = "color pixel() { return color(1.0, 0.0, 0.0, 1.0); }";
    std::string expectedTree = ParserTestLoader::trim(R"(
AstProgram(
  items: [
    AstFunction(
      name: 'pixel',
      returnType: 'color',
      params: [],
      body: AstBlock(
        items: [
          AstReturnStatement(
            exp: AstStructInit(
              type: 'color',
              args: [
                AstConstantExp(
                  type: 'float',
                  value: '1'
                ),
                AstConstantExp(
                  type: 'float',
                  value: '0'
                ),
                AstConstantExp(
                  type: 'float',
                  value: '0'
                ),
                AstConstantExp(
                  type: 'float',
                  value: '1'
                )
              ]
            )
          )
        ]
      )
    )
  ]
)
)");

    Lexer lexer(src);
    Parser parser(lexer, allocator, commonAllocator);
    for (const auto& type : typeTable) {
        parser.declareType(type.first);
    }

    AstProgram* program = parser.parseProgram();
    Semantic(symbolTable, typeTable, idGen, allocator).process(program);

    std::ostringstream actualTree;
    AstPrinter(actualTree).print(program);
    REQUIRE(actualTree.str() == expectedTree);
}