#pragma once
#include <unordered_map>
#include "sparkc/symbol/SymbolType.h"

class BuildResult {
public:
    class Function {
    public:
        Function(void* ptr, StringRef name, SymbolFunctionType* type)
            : ptr(ptr)
            , name(name)
            , type(type) { }

        void* getPointer() const { return ptr; }
        const StringRef& getName() const { return name; }
        SymbolFunctionType* getType() const { return type; }

    private:
        void* ptr;
        StringRef name;
        SymbolFunctionType* type;
    };

    BuildResult() = default;
    BuildResult(size_t binarySize, const std::unordered_map<StringRef, Function>& functions)
        : binarySize(binarySize)
        , functions(functions) { }

    size_t getBinarySize() const {
        return binarySize;
    }

    Function* lookupFunction(StringRef name) {
        auto it = functions.find(name);
        if (it != functions.end()) {
            return &it->second;
        }
        return nullptr;
    }

private:
    size_t binarySize;
    std::unordered_map<StringRef, Function> functions;
};