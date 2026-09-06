#pragma once
#include <unordered_map>
#include <optional>
#include "sparkc/symbol/SymbolType.h"

class BuildResult {
public:
    class Function {
    public:
        Function(uint32_t offset, StringRef name, SymbolFunctionType* type)
            : offset(offset)
            , name(name)
            , type(type) { }

        uint32_t getOffset() const { return offset; }
        const StringRef& getName() const { return name; }
        SymbolFunctionType* getType() const { return type; }

    private:
        uint32_t offset;
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

    const std::unordered_map<StringRef, Function>& getFunctions() const {
        return functions;
    }

private:
    void* binary;
    size_t binarySize;
    std::unordered_map<StringRef, Function> functions;
};