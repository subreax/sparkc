#pragma once
#include <unordered_map>
#include <functional>
#include "RvaValue.h"
#include "../../common/StringRef.h"
#include "../../common/alloc/Allocator.h"

class StackFrame {
public:
    StackFrame(Allocator& rvaAlloc) : rvaAlloc(rvaAlloc) {  }

    void occupy(int bytes) {
        if (localSize == 0) {
            localSize = 4; // s0
        }

        localSize += bytes;
    }

    RvaMemory* allocate(int bytes) {
        occupy(bytes);
        return rvaAlloc.create<RvaMemory>(RvReg::S0, -localSize);
    }

    RvaMemory* getOrPush(StringRef id, int size = 4, int offset = 0) {
        auto it = var2stack.find(id);
        RvaMemory* mem;
        if (it != var2stack.end()) {
            mem = it->second;
        } else {
            auto* rvaMem = allocate(size);
            var2stack[id] = rvaMem;
            mem = rvaMem;
        }

        if (offset == 0) {
            return mem;
        }
        else {
            return rvaAlloc.create<RvaMemory>(mem->getBase(), mem->getOffset() + offset);
        }
    }

    void save() {
        savedLocalSize = localSize;
    }

    void restore() {
        maxSize = std::max(maxSize, localSize + spArgsSize);
        localSize = savedLocalSize;
        spArgsSize = 0;
    }

    RvaMemory* pushArg() {
        occupy(4);
        auto* mem = rvaAlloc.create<RvaMemory>(RvReg::SP, spArgsSize);
        spArgsSize += 4;
        return mem;
    }

    int getSize() const {
        return std::max(maxSize, localSize + spArgsSize);
    }
    int getSizeAligned16() const { return ((getSize() + 15) / 16) * 16; }

private:
    std::unordered_map<StringRef, RvaMemory*> var2stack;
    Allocator& rvaAlloc;
    int localSize = 0;
    int spArgsSize = 0;
    int maxSize = 0;
    int savedLocalSize = 0;
};