#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include "sparkc/common/alloc/Allocator.h"
#include "sparkc/symbol/SymbolType.h"
#include "sparkc/symbol/SymbolTable.h"
#include "sparkc/type/TypeTable.h"
#include "sparkc/common/IdentifierGen.h"
#include "sparkc/common/LabelGen.h"
#include "sparkc/size/SymbolSize.h"
#include "sparkc/backend/rv/asm/RvAssembler.h"

struct SparkCompilerContext {
    SparkCompilerContext(Allocator& shared, uint8_t* out, size_t outCap)
        : symTable(shared)
        , typeTable(shared)
        , symSize(symTable, typeTable)
        , idGen(shared)
        , labelGen(shared)
        , assembler(out, outCap) { }

    SymbolTable symTable;
    TypeTable typeTable;
    SymbolSize symSize;
    IdentifierGen idGen;
    LabelGen labelGen;
    RvAssembler assembler;
};