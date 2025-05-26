#pragma once
#include <iostream>
#include "../spark/common/BoundArray.h"

class BinaryPrinter {
public:
    static void print(std::ostream& os, BoundArray<uint32_t> data) {
        char buf[16];
        for (size_t i = 0; i < data.size(); i++) {
            sprintf(buf, "%08x\n", data[i]);
            os << buf;
        }
    }
};