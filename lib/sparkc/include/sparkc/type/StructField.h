#pragma once
#include "sparkc/symbol/SymbolType.h"

struct StructField {
    StructField(StringRef name, SymbolType* type, size_t offset = 0)
        : name(name)
        , type(type)
        , offset(offset) { }

    StructField(const char* name, SymbolType* type)
        : StructField(StringRef::cstr(name), type) { }

    StringRef name;
    SymbolType* type;
    size_t offset;
};