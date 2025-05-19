#pragma once
#include "LinearAllocator.h"
#include "StringUtils.h"

class LabelGen {
public:
    LabelGen(LinearAllocator& allocator) : allocator(allocator) {  }

    const char* uniqueInternal(const char* id) {
        auto len = generateInternal(id);
        char* generated = (char*) allocator.allocate(len);
        StringUtils::copy(generated, buf, len);
        return generated;
    }

    static constexpr int LABEL_MAX_LEN = 24;

private:
    int generateInternal(const char* id) {
        char* generated = buf;
        generated += StringUtils::copy(generated, ".L", 2);
        generated += StringUtils::copy(generated, id, LABEL_MAX_LEN);
        generated += sprintf(generated, "_%d", counter);
        return generated - buf + 1;
    }

    LinearAllocator& allocator;
    int counter = 0;
    char buf[LABEL_MAX_LEN + 16];
};
