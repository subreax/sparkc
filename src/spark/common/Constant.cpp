#include "Constant.h"

int32_t Constant::intValue() const {
    return ((IntConstant*) this)->val;
}

float Constant::floatValue() const {
    return ((FloatConstant*) this)->val;
}

bool Constant::operator==(const Constant& other) const {
    if (type->kind != other.type->kind) {
        return false;
    }

    if (type->kind == SymbolType::Kind::Integer) {
        return intValue() == other.intValue();
    }
    else if (type->kind == SymbolType::Kind::Float) {
        return floatValue() == other.floatValue();
    } 
    else {
        sparkError("Constant", "Unknown symbol type: %d", type->kind);
        return false;
    }
}