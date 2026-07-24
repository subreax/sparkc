#include "sparkc/frontend/ast/exp/AstExp.h"
#include <string>

static const char* KIND_STRING_VALUES[] = {
#define X(kind, name) name,
    SPARK_AST_EXP_KINDS_LIST(X)
#undef X
};

static constexpr size_t KINDS_COUNT = (size_t) AstExp::Kind::_Count;

const char* AstExp::kindToString(AstExp::Kind kind) {
    size_t kind1 = (size_t) kind;
    if (kind1 < KINDS_COUNT) {
        return KIND_STRING_VALUES[kind1];
    }
    sparkError("AstExp", "Unknown AstExp::Kind %d", kind1);
    return "";
}
