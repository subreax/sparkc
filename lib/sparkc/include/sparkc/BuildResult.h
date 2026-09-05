#pragma once
#include <unordered_map>
#include <optional>
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
    BuildResult(void* binary, size_t binarySize, const std::unordered_map<StringRef, Function>& functions)
        : binary(binary)
        , binarySize(binarySize)
        , functions(functions) { }

    void* getBinary() {
        return binary;
    }

    size_t getBinarySize() const {
        return binarySize;
    }

    std::optional<Function> lookupFunction(StringRef name) {
        auto it = functions.find(name);
        if (it != functions.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    std::optional<Function> lookupFunction(const char* name) {
        return lookupFunction(StringRef::cstr(name));
    }

private:
    void* binary;
    size_t binarySize;
    std::unordered_map<StringRef, Function> functions;
};