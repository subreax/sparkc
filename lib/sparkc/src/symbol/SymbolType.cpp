#include "sparkc/symbol/SymbolType.h"
#include "sparkc/common/Error.h"

std::string SymbolType::toString() const {
    switch (kind) {
    case SymbolType::Kind::Integer: return "int";
    case SymbolType::Kind::Float: return "float";

    case SymbolType::Kind::Pointer: {
        auto* refType = (SymbolPointerType*) this;
        return refType->getVarType()->toString() + "*";
    }

    case SymbolType::Kind::Structure: {
        auto* structType = (SymbolStructureType*) this;
        return structType->getTag().toString();
    }

    default:
        sparkError("SymbolType", "Unknown SymbolType::Kind: %d", kind);
        return "";
    }
}
