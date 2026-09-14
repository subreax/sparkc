#pragma once
#include "sparkc/common/alloc/Allocator.h"
#include "sparkc/symbol/SymbolTable.h"
#include "sparkc/type/TypeTable.h"
#include "sparkc/size/SymbolSize.h"
#include "sparkc/common/IdentifierGen.h"
#include "sparkc/common/LabelGen.h"
#include "sparkc/backend/rv/asm/RvAssembler.h"

class SparkBuildContext {
public:
    SparkBuildContext(Allocator& sharedAlloc, MemBlockRef outBin)
        : symTable(sharedAlloc)
        , typeTable(sharedAlloc)
        , symSize(symTable, typeTable)
        , idGen(sharedAlloc)
        , labelGen(sharedAlloc)
        , assembler(outBin) { }

    SymbolTable symTable;
    TypeTable typeTable;
    SymbolSize symSize;
    IdentifierGen idGen;
    LabelGen labelGen;
    RvAssembler assembler;

    std::vector<SkrStaticVar*> skrStaticVars;
};