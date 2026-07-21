#pragma once
#include <cstdint>
#include "sparkc/common/StringRef.h"

struct Label {
    Label() = default;
    Label(int32_t offset, StringRef value)
        : offset(offset)
        , value(value) { }

    int32_t offset = 0;
    StringRef value = StringRef::nullInstance();
};