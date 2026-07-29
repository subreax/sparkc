#include "sparkc/skr/instr/SkrBinary.h"

const char* SkrBinary::operatorStringValue(Operator op) {
    static const char* STRING_VALUES[] = {
#define X(id, name) name,
        SKR_BINARY_OP_LIST(X)
#undef X
    };

    static constexpr size_t count = (size_t) Operator::_Count;

    auto idx = (size_t) op;
    if (idx < count) {
        return STRING_VALUES[idx];
    }

    sparkError("SkrBinary", "operatorStringValue: unknown operator");
    return "";
}