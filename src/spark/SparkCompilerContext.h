#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include "common/alloc/Allocator.h"
#include "symbol/SymbolType.h"
#include "symbol/SymbolTable.h"
#include "type/TypeTable.h"
#include "common/IdentifierGen.h"
#include "common/LabelGen.h"
#include "size/SymbolSize.h"
#include "backend/rv/asm/RvAssembler.h"

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