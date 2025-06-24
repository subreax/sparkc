#pragma once
#include "../spark/symbol/SymbolType.h"

class Type2String {
public:
    static std::string run(const SymbolType* t) {
        if (t == nullptr) {
            return "null";
        }

        switch (t->kind) {
        case SymbolType::Kind::Integer: return "int";
        case SymbolType::Kind::Float: return "float";
        case SymbolType::Kind::Pointer: {
            auto* refType = (SymbolPointerType*) t;
            return run(refType->getVarType()) + std::string("*");
        }
        default: return "<unknown>";
        }
    }
};