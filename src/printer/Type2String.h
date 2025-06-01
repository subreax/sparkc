#pragma once
#include "../spark/symbol/SymbolType.h"

class Type2String {
public:
    static const char* run(const SymbolType* t) {
        if (t == nullptr) {
            return "null";
        }

        switch (t->kind) {
        case SymbolType::Kind::Integer: return "int";
        case SymbolType::Kind::Float: return "float";
        default: return "<unknown>";
        }
    }
};