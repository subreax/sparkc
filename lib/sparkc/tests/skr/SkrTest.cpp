#include <catch2/catch_test_macros.hpp>
#include "skr/SkrEmitter.h"
#include "sparkc/frontend/ast/AstFactory.h"
#include "sparkc/common/alloc/LinearAllocator.h"
#include "sparkc/frontend/Frontend.h"
#include "utils/MatchTestRunner.h"
#include "sparkc/skr/SkrPrinter.h"
#include "TestOptions.h"

extern TestOptions testOptions;

static std::string buildSkr(const std::string& src) {
    LinearAllocator shared("shared", 2048);
    LinearAllocator pool1("pool1", 2048);
    LinearAllocator pool2("pool2", 2048);
    SymbolTable symbolTable(shared);
    TypeTable typeTable(shared);
    IdentifierGen idGen(shared);
    LabelGen labelGen(shared);

    AstFactory astf(pool1);
    Frontend frontend(src.c_str(), astf, symbolTable, typeTable, idGen);
    std::vector<SkrInstruction*> skrs;
    SkrPrinter skrPrinter(symbolTable, false);

    while (frontend.hasNext()) {
        auto* astItem = frontend.processNextItem();
        if (astItem->kind == AstProgItem::Kind::Function) {
            auto* skrFun = SkrEmitter::emit((AstFunction*) astItem, pool2, symbolTable, typeTable, idGen, labelGen, skrs);
            skrPrinter.append(skrFun);
        }
    }
    return skrPrinter.toString();
}

TEST_CASE("Skr Match Tests", "[skr]") {
    MatchTestRunner::runAllTests(testOptions.skrMatchTestsPath, buildSkr);
}