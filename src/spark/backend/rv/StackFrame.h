#pragma once
#include <map>
#include <functional>
#include "RvaValue.h"
#include "../../common/LinearAllocator.h"

class StackFrame {
public:
    StackFrame(LinearAllocator& rvaAlloc) : rvaAlloc(rvaAlloc) {  }

    int allocate(int bytes) {
        if (size == 0) {
            size = 4; // s0
        }

        size += bytes;
        return -size;
    }

    RvaMemory* getOrPush(const char* id) {
        auto it = var2stack.find(id);
        if (it != var2stack.end()) {
            return it->second;
        } else {
            auto* rvaMem = rvaAlloc.create<RvaMemory>(RvReg::S0, allocate(4));
            var2stack.emplace(id, rvaMem);
            return rvaMem;
        }
    }

    int getSize() const { return size; }
    int getSizeAligned16() const { return ((size + 15) / 16) * 16; }

private:
    std::map<const char*, RvaMemory*> var2stack;
    LinearAllocator& rvaAlloc;
    int size = 0;
};