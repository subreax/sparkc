#pragma once
#include <unordered_map>
#include <functional>
#include "RvaValue.h"
#include "../../common/StringRef.h"
#include "../../common/alloc/LinearAllocator.h"

class StackFrame {
public:
    StackFrame(LinearAllocator& rvaAlloc) : rvaAlloc(rvaAlloc) {  }

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

    RvaMemory* getOrPush(const char* id) {
        StringRef idRef(id, StringRef::lengthOf(id));
        auto it = var2stack.find(idRef);
        if (it != var2stack.end()) {
            return it->second;
        } else {
            auto* rvaMem = allocate(4);
            var2stack[idRef] = rvaMem;
            return rvaMem;
        }
    }

    RvaMemory* pushArg() {
        occupy(4);
        auto* mem = rvaAlloc.create<RvaMemory>(RvReg::SP, argsSize);
        argsSize += 4;
        return mem;
    }

    void bindParam(const char* param) {
        StringRef paramRef(param, StringRef::lengthOf(param));
        var2stack[paramRef] = rvaAlloc.create<RvaMemory>(RvReg::S0, boundParamsOffset);
        boundParamsOffset += 4;
    }

    void popArgs() {
        maxSize = std::max(maxSize, localSize);
        localSize -= argsSize;
        argsSize = 0;
    }

    int getSize() const { return localSize; }
    int getSizeAligned16() const { return ((localSize + 15) / 16) * 16; }

private:
    std::unordered_map<StringRef, RvaMemory*> var2stack;
    LinearAllocator& rvaAlloc;
    int localSize = 0;
    int argsSize = 0;
    int maxSize = 0;
    int boundParamsOffset = 0;
};