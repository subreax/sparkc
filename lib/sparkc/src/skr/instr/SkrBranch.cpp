#include "sparkc/skr/instr/SkrBranch.h"

const char* SkrBranch::operatorStringValue(Operator op) {
    static const char* STRING_VALUES[] = {
#define X(id, name) name,
        SKR_BRANCH_OP_LIST(X)
#undef X
    };

    static constexpr size_t count = (size_t) Operator::_Count;

    auto idx = (size_t) op;
    if (idx < count) {
        return STRING_VALUES[idx];
    }

    sparkError("SkrBranch", "operatorStringValue: unknown operator");
    return "";
}