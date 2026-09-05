#include "sparkc/common/ExpandableStringBuilder.h"

ExpandableStringBuilder& ExpandableStringBuilder::append(const char* str) {
    size_t len = strnlen(str, 2048);
    return append(str, len);
}