#include "SkrValue.h"
#include <cstring>

bool SkrValue::operator==(const SkrValue& other) const {
    if (kind != other.kind) {
        return false;
    }

    if (kind == Kind::Var) {
        return toSkrVar()->getId() == other.toSkrVar()->getId();
    }
    else if (kind == Kind::Const) {
        return *(toSkrConst()->getConst()) == *(other.toSkrConst()->getConst());
    }
    else {
        sparkError("SkrValue", "Unknown SkrValue kind: %d", kind);
        return false;
    }
}

bool SkrValue::operator!=(const SkrValue& other) const {
    return !(*this == other);
}
