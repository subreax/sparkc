#pragma once
#include <unordered_map>
#include "sparkc/symbol/SymbolType.h"
#include "sparkc/symbol/SymbolTypeFactory.h"

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
    BuildResult(
        uint8_t* binary, 
        size_t binarySize, 
        const std::unordered_map<StringRef, Function>& functions
    )
        : binary(binary)
        , binarySize(binarySize)
        , functions(functions) { }

    uint8_t* getBinary() {
        return binary;
    }

    size_t getBinarySize() const {
        return binarySize;
    }

    template<typename T>
    T lookupFunction(const char* name, SymbolType* returnType, std::initializer_list<SymbolType*> params) {
        auto* fun = _lookupFunction(name, returnType, params);
        if (fun == nullptr) {
            return nullptr;
        }
        return reinterpret_cast<T>(binary + fun->getOffset());
    }

    const std::unordered_map<StringRef, Function>& getFunctions() const {
        return functions;
    }

private:
    bool isFunctionMatches(const Function& fun, const char* name, SymbolType* returnType, const std::vector<SymbolType*>& params) {
        if (fun.getName() != name) {
            return false;
        }

        BoundArray<SymbolType*> boundParams(memBlockRefOf(params));
        SymbolFunctionType fnType(returnType, boundParams);
        return *fun.getType() == fnType;
    }

    template<typename T>
    static MemBlockRef memBlockRefOf(const std::vector<T>& v) {
        return MemBlockRef(
            v.size() * sizeof(T), 
            (uint8_t*) v.data()
        );
    }

    const Function* _lookupFunction(const char* name, SymbolType* returnType, std::initializer_list<SymbolType*> params) {
        std::vector<SymbolType*> paramsV(std::move(params));
        for (const auto& [funName, fun] : functions) {
            if (isFunctionMatches(fun, name, returnType, paramsV)) {
                return &fun;
            }
        }
        return nullptr;
    }

    uint8_t* binary;
    size_t binarySize;
    std::unordered_map<StringRef, Function> functions;
};