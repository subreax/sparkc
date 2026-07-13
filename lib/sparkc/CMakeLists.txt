add_library(sparkc)

set(SPARKC_SRC
  src/SparkCompiler.cpp
  src/backend/rv/asm/RvAssembler.cpp
  src/backend/rv/asm/RvListing.cpp
  src/backend/rv/asm/Rv32Base.cpp
  src/backend/rv/instr/RvaBeginTempStack.cpp
  src/backend/rv/instr/RvaBinary.cpp
  src/backend/rv/instr/RvaBranch.cpp
  src/backend/rv/instr/RvaCall.cpp
  src/backend/rv/instr/RvaEndTempStack.cpp
  src/backend/rv/instr/RvaEpilogue.cpp
  src/backend/rv/instr/RvaGetAddress.cpp
  src/backend/rv/instr/RvaJump.cpp
  src/backend/rv/instr/RvaLabel.cpp
  src/backend/rv/instr/RvaLoad.cpp
  src/backend/rv/instr/RvaMov.cpp
  src/backend/rv/instr/RvaPrologue.cpp
  src/backend/rv/instr/RvaReserveOnStack.cpp
  src/backend/rv/instr/RvaRet.cpp
  src/backend/rv/instr/RvaStore.cpp
  src/common/Constant.cpp
  src/common/StringRef.cpp
  src/frontend/lexer/Lexer.cpp
  src/frontend/lexer/Token.cpp
  src/frontend/lexer/TokenReaders.cpp
  src/frontend/parser/Parser.cpp
  src/frontend/parser/ast/AstPrinter.cpp
  src/frontend/semantic/IdentifierResolution.cpp
  src/frontend/semantic/Scope.cpp
  src/frontend/semantic/TypeChecker.cpp
  src/skr/SkrEmitter.cpp
  src/skr/value/SkrValue.cpp
  src/skr/optimizer/SkrCfgOverloadings.cpp
  src/skr/optimizer/copyprop/CopyPropagation.cpp
  src/symbol/SymbolType.cpp
)

target_sources(sparkc PRIVATE ${SPARKC_SRC})
target_include_directories(sparkc PUBLIC include/)


find_package(Catch2 3 REQUIRED)
add_executable(tests)
target_sources(tests 
  PRIVATE 
    tests/frontend/parser/AstTester.cpp
    tests/frontend/parser/ParserTest.cpp
    tests/frontend/parser/ParserTestLoader.cpp
    tests/frontend/parser/ExternalStructDeclarationTest.cpp
    tests/alloc/test_linearalloc.cpp
    tests/asm/test_rv32base.cpp
    tests/rva/RvaMovTest.cpp
)
target_include_directories(tests PRIVATE
  src/
  tests/
)

target_link_libraries(tests PRIVATE sparkc Catch2::Catch2WithMain)
