#pragma once
#include "sparkc/symbol/SymbolType.h"

struct StructField {
    StructField(StringRef name, SymbolType* type, size_t offset = 0)
        : name(name)
        , type(type)
        , offset(offset) { }

    StringRef name;
    SymbolType* type;
    size_t offset;
};