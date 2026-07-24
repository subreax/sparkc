#pragma once
#include <vector>
#include "sparkc/common/alloc/Allocator.h"
#include "sparkc/symbol/SymbolType.h"

class SymbolTypeFactory {
public:
    SymbolTypeFactory(Allocator& allocator)
        : allocator(allocator) { }

    SymbolIntType* int_() {
        return SymbolIntType::getInstance();
    }

    SymbolFloatType* float_() {
        return SymbolFloatType::getInstance();
    }

    SymbolFunctionType* function(SymbolType* returnType, const std::vector<SymbolType*>& params) {
        auto paramsBA = BoundArray<SymbolType*>::fromVector(params, allocator);
        return allocator.create<SymbolFunctionType>(returnType, paramsBA);
    }

    SymbolPointerType* pointer(SymbolType* valueType) {
        return allocator.create<SymbolPointerType>(valueType);
    }

    SymbolStructureType* structure(StringRef tag) {
        return allocator.create<SymbolStructureType>(tag);
    }

private:
    Allocator& allocator;
};
