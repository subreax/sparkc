#pragma once
#include "LinearAllocator.h"
#include "CStringBuilder.h"

class LabelGen {
public:
    LabelGen(LinearAllocator& allocator) : allocator(allocator) {  }

    const char* uniqueInternal(const char* id) {
        auto len = CStringBuilder(buf, sizeof(buf))
            .append(".")
            .append(id, LABEL_MAX_LEN)
            .append("_")
            .append(counter)
            .getLengthWith0();

        counter++;

        char* generated = (char*) allocator.allocate(len);
        CStringBuilder(generated, len).append(buf);
        return generated;
    }

    static constexpr int LABEL_MAX_LEN = 24;

private:
    LinearAllocator& allocator;
    int counter = 0;
    char buf[LABEL_MAX_LEN + 16];
};
