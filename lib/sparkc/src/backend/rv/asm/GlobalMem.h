#pragma once
#include <vector>
#include "sparkc/common/StringRef.h"
#include "sparkc/size/SymbolSize.h"
#include "sparkc/common/alloc/MemBlockRef.h"
#include "sparkc/common/alloc/LinearAllocator.h"

class GlobalMem {
public:
    uint8_t* allocateVar(StringRef id) {
        size_t size = ss.get(id);
        if (getFreeMem() >= size) {
            auto* ptr = block.mem + size;
            allocated += size;
            return ptr;
        }

        sparkError("GlobalMem", "Not enough memory to place global variable '" + id.toString() + "'");
        return nullptr;
    }

private:
    size_t getFreeMem() const {
        if (block.sz >= allocated) {
            return block.sz - allocated;
        }

        sparkError("GlobalMem", "Out of bounds");
        return 0;
    }

    SymbolSize& ss;
    MemBlockRef block;
    size_t allocated = 0;
};