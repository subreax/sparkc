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

bool operator==(const SymbolType& t1, const SymbolType& t2) {
    if (t1.kind != t2.kind) {
        return false;
    }

    if (t1.kind == SymbolType::Kind::Integer || t1.kind == SymbolType::Kind::Float) {
        return true;
    }

    if (t1.kind == SymbolType::Kind::Structure) {
        auto* pt1 = static_cast<const SymbolStructureType*>(&t1);
        auto* pt2 = static_cast<const SymbolStructureType*>(&t2);
        return pt1->getTag() == pt2->getTag();
    }

    if (t1.kind == SymbolType::Kind::Pointer) {
        auto* pt1 = static_cast<const SymbolPointerType*>(&t1);
        auto* pt2 = static_cast<const SymbolPointerType*>(&t2);
        return *pt1->getVarType() == *pt2->getVarType();
    }

    if (t1.kind == SymbolType::Kind::Function) {
        auto* pt1 = static_cast<const SymbolFunctionType*>(&t1);
        auto* pt2 = static_cast<const SymbolFunctionType*>(&t2);

        if (*pt1->getReturnType() != *pt2->getReturnType()) {
            return false;
        }

        if (pt1->getParams().size() != pt2->getParams().size()) {
            return false;
        }

        size_t paramsCount = pt1->getParams().size();
        for (size_t i = 0; i < paramsCount; i++) {
            auto* paramType1 = pt1->getParams()[i];
            auto* paramType2 = pt2->getParams()[i];
            if (*paramType1 != *paramType2) {
                return false;
            }
        }
        return true;
    }

    sparkError("SymbolType", "operator== Unknown SymbolType::Kind: %d", (int) t1.kind);
    return false;
}

bool operator!=(const SymbolType& t1, const SymbolType& t2) {
    return !(t1 == t2);
}
