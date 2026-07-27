#pragma once
#include <vector>
#include "sparkc/symbol/SymbolTypeFactory.h"

struct SparkExtStructField {
    const char* name;
    SymbolType* type;
};

class SparkInitContext {
public:
    SparkInitContext(SymbolTypeFactory& types)
        : _types(types) { }

    virtual ~SparkInitContext() = default;

    virtual void bindFunction(
        void* ptr,
        const char* name,
        SymbolType* type,
        std::initializer_list<SymbolType*> params
    ) = 0;

    virtual void addStruct(
        const char* tag,
        std::initializer_list<SparkExtStructField> fields
    ) = 0;

    SymbolTypeFactory& types() {
        return _types;
    }

private:
    SymbolTypeFactory& _types;
};
