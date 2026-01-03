#pragma once
#include "sparkc/backend/rv/asm/RvAssembler.h"
#include "sparkc/common/IdentifierGen.h"
#include "sparkc/common/LabelGen.h"
#include "sparkc/common/alloc/Allocator.h"
#include "sparkc/size/SymbolSize.h"
#include "sparkc/symbol/SymbolTable.h"
#include "sparkc/symbol/SymbolType.h"
#include "sparkc/type/TypeTable.h"
#include <cstdint>
#include <string>
#include <vector>

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