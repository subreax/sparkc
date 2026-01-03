add_library(sparkc)

include(CMakePrintHelpers)
Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.8.1
)

FetchContent_MakeAvailable(Catch2)


set(SPARKC_INCL_PUB
  include/sparkc/SparkBuildInfo.h
  include/sparkc/SparkCompiler.h
  include/sparkc/SparkCompilerContext.h
  include/sparkc/SparkPools.h
  include/sparkc/SparkMemoryPercentUsage.h
  include/sparkc/backend/rv/asm/RvAssembler.h
  include/sparkc/backend/rv/RvaValue.h
  include/sparkc/backend/rv/RvReg.h
  include/sparkc/backend/rv/asm/RvListing.h
  include/sparkc/backend/rv/instr/everything.h
  include/sparkc/backend/rv/instr/RvaBeginTempStack.h
  include/sparkc/backend/rv/instr/RvaBinary.h
  include/sparkc/backend/rv/instr/RvaBranch.h
  include/sparkc/backend/rv/instr/RvaCall.h
  include/sparkc/backend/rv/instr/RvaEndTempStack.h
  include/sparkc/backend/rv/instr/RvaEpilogue.h
  include/sparkc/backend/rv/instr/RvaGetAddress.h
  include/sparkc/backend/rv/instr/RvaInstruction.h
  include/sparkc/backend/rv/instr/RvaJump.h
  include/sparkc/backend/rv/instr/RvaLabel.h
  include/sparkc/backend/rv/instr/RvaLoad.h
  include/sparkc/backend/rv/instr/RvaMov.h
  include/sparkc/backend/rv/instr/RvaPrologue.h
  include/sparkc/backend/rv/instr/RvaReserveOnStack.h
  include/sparkc/backend/rv/instr/RvaRet.h
  include/sparkc/backend/rv/instr/RvaStore.h
  include/sparkc/common/BoundArray.h
  include/sparkc/common/Constant.h
  include/sparkc/common/Error.h
  include/sparkc/common/IdentifierGen.h
  include/sparkc/common/LabelGen.h
  include/sparkc/common/OutOfBoundsException.h
  include/sparkc/common/SparkRuntimeException.h
  include/sparkc/common/Stack.h
  include/sparkc/common/StringBuilder.h
  include/sparkc/common/StringRef.h
  include/sparkc/common/alloc/Allocator.h
  include/sparkc/common/alloc/LinearAllocator.h
  include/sparkc/common/alloc/MemBlock.h
  include/sparkc/common/alloc/NoMemoryException.h
  include/sparkc/common/alloc/StatAllocator.h
  include/sparkc/common/cfg/BitArray.h
  include/sparkc/common/cfg/BitMatrix.h
  include/sparkc/common/cfg/CfgBlock.h
  include/sparkc/common/cfg/CfgBuilder.h
  include/sparkc/common/cfg/CfGraph.h
  include/sparkc/frontend/ast/AstBlock.h
  include/sparkc/frontend/ast/AstBlockItem.h
  include/sparkc/frontend/ast/AstFunction.h
  include/sparkc/frontend/ast/AstFunParam.h
  include/sparkc/frontend/ast/AstProgItem.h
  include/sparkc/frontend/ast/AstProgram.h
  include/sparkc/frontend/ast/AstStruct.h
  include/sparkc/frontend/ast/everything.h
  include/sparkc/frontend/ast/declaration/AstDeclaration.h
  include/sparkc/frontend/ast/declaration/AstVarDeclaration.h
  include/sparkc/frontend/ast/exp/AstAddrOf.h
  include/sparkc/frontend/ast/exp/AstAssignment.h
  include/sparkc/frontend/ast/exp/AstBinaryExp.h
  include/sparkc/frontend/ast/exp/AstCast.h
  include/sparkc/frontend/ast/exp/AstConstantExp.h
  include/sparkc/frontend/ast/exp/AstDereference.h
  include/sparkc/frontend/ast/exp/AstDot.h
  include/sparkc/frontend/ast/exp/AstExp.h
  include/sparkc/frontend/ast/exp/AstFunCall.h
  include/sparkc/frontend/ast/exp/AstStructInit.h
  include/sparkc/frontend/ast/exp/AstVar.h
  include/sparkc/frontend/ast/exp/everything.h
  include/sparkc/frontend/ast/printer/AstPrinter.h
  include/sparkc/frontend/ast/statement/AstCompoundStatement.h
  include/sparkc/frontend/ast/statement/AstExpressionStatement.h
  include/sparkc/frontend/ast/statement/AstIfStatement.h
  include/sparkc/frontend/ast/statement/AstReturnStatement.h
  include/sparkc/frontend/ast/statement/AstStatement.h
  include/sparkc/frontend/ast/statement/AstWhileStatement.h
  include/sparkc/frontend/ast/statement/everything.h
  include/sparkc/frontend/lexer/Token.h
  include/sparkc/frontend/parser/except/DuplicateDeclarationException.h
  include/sparkc/frontend/parser/except/everything.h
  include/sparkc/frontend/parser/except/ParseConstException.h
  include/sparkc/frontend/parser/except/ParseException.h
  include/sparkc/frontend/parser/except/UndeclaredVariableException.h
  include/sparkc/frontend/parser/except/UnexpectedTokenExcepion.h
  include/sparkc/frontend/parser/except/UnknownTypeException.h
  include/sparkc/frontend/parser/except/WrongExprException.h
  include/sparkc/frontend/parser/except/WrongStatementException.h
  include/sparkc/size/SymbolSize.h
  include/sparkc/skr/SkrFunction.h
  include/sparkc/skr/instr/everything.h
  include/sparkc/skr/instr/SkrBinary.h
  include/sparkc/skr/instr/SkrBranch.h
  include/sparkc/skr/instr/SkrCopy.h
  include/sparkc/skr/instr/SkrCopyFromOffset.h
  include/sparkc/skr/instr/SkrCopyToOffset.h
  include/sparkc/skr/instr/SkrFloat2Int.h
  include/sparkc/skr/instr/SkrFunCall.h
  include/sparkc/skr/instr/SkrGetAddr.h
  include/sparkc/skr/instr/SkrInstruction.h
  include/sparkc/skr/instr/SkrInt2Float.h
  include/sparkc/skr/instr/SkrJump.h
  include/sparkc/skr/instr/SkrLabel.h
  include/sparkc/skr/instr/SkrLoad.h
  include/sparkc/skr/instr/SkrStore.h
  include/sparkc/skr/optimizer/SkrOptimizerConfig.h
  include/sparkc/skr/optimizer/SkrOptOnCfgCreatedListener.h
  include/sparkc/skr/value/SkrValue.h
  include/sparkc/symbol/SymbolTable.h
  include/sparkc/symbol/SymbolType.h
  include/sparkc/symbol/except/DuplicateSymbolDeclarationException.h
  include/sparkc/symbol/except/UndeclaredSymbolException.h
  include/sparkc/type/TypeTable.h
)



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

set(SPARKC_INCL_PRIV
  src/backend/rv/FixedUtils.h
  src/backend/rv/RvaFixer.h
  src/backend/rv/RvaPseudoReplacer.h
  src/backend/rv/Skr2RvaPseudo.h
  src/backend/rv/StackFrame.h
  src/backend/rv/asm/Rv32Base.h
  src/backend/rv/asm/Rv32I.h
  src/backend/rv/asm/Rv32M.h
  src/frontend/lexer/Lexer.h
  src/frontend/lexer/TokenReaders.h
  src/frontend/parser/Parser.h
  src/frontend/parser/ast/Printer.h
  src/frontend/semantic/IdentifierResolution.h
  src/frontend/semantic/Scope.h
  src/frontend/semantic/Semantic.h
  src/frontend/semantic/TypeChecker.h
  src/frontend/semantic/TypeException.h
  src/skr/SkrEmitter.h
  src/skr/optimizer/ConstantFolding.h
  src/skr/optimizer/SkrOptimizer.h
  src/skr/optimizer/SkrOptimizerUtils.h
  src/skr/optimizer/Uniqueue.h
  src/skr/optimizer/UnreachableCodeElimination.h
  src/skr/optimizer/copyprop/CopyPropagation.h
  src/skr/optimizer/copyprop/RCABlock.h
  src/skr/optimizer/copyprop/ReachingCopies.h
  src/skr/optimizer/copyprop/ReachingCopiesAnalysis.h
  src/skr/optimizer/copyprop/ReachingCopiesUtils.h
  src/skr/optimizer/dselim/DeadStoreElimination.h
  src/skr/optimizer/dselim/DSEBlock.h
  src/skr/optimizer/dselim/LivenessAnalysis.h
  src/skr/optimizer/dselim/VarSet.h
  src/skr/value/SkrExpRes.h
)


target_sources(sparkc 
  PRIVATE ${SPARKC_SRC}

  PUBLIC
    FILE_SET pubHeaders
    TYPE HEADERS
    BASE_DIRS
      include
    FILES
      ${SPARKC_INCL_PUB}

  PRIVATE
    FILE_SET privHeaders
    TYPE HEADERS
    BASE_DIRS
      src
    FILES
      ${SPARKC_INCL_PRIV}
)



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

  PRIVATE
    FILE_SET privHeaders
      TYPE HEADERS
      BASE_DIRS
        src
      FILES
        ${SPARKC_INCL_PRIV}
)

target_link_libraries(tests PRIVATE sparkc)
target_include_directories(tests PRIVATE tests/)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)