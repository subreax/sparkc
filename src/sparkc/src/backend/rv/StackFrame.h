#pragma once
#include "sparkc/backend/rv/RvaValue.h"
#include "sparkc/common/StringRef.h"
#include "sparkc/common/alloc/Allocator.h"
#include <functional>
#include <unordered_map>
#include <unordered_set>

class StackFrame {
public:
    StackFrame(Allocator& rvaAlloc) : rvaAlloc(rvaAlloc) {}

    void occupy(int bytes) {
        if (s0size == 0) {
            s0size = 4; // save s0
        }

        s0size += bytes;
    }

    RvaMemory* getOrPush(StringRef id, int size = 4, int offset = 0) {
        auto it = var2stack.find(id);
        RvaMemory* mem;
        if (it != var2stack.end()) {
            mem = it->second;
        }
        else {
            if (isTemp) {
                temporaries.emplace(id);
            }
            auto* rvaMem = allocate(size);
            var2stack[id] = rvaMem;
            mem = rvaMem;
        }

        if (offset == 0) {
            return mem;
        }
        else {
            return rvaAlloc.create<RvaMemory>(
                mem->getBase(), mem->getOffset() + offset);
        }
    }

    void save() {
        savedS0Size = s0size;
        isTemp = true;
    }

    void restore() {
        for (const StringRef& tempId : temporaries) {
            var2stack.erase(tempId);
        }
        temporaries.clear();

        maxSize = std::max(maxSize, s0size + spArgsSize);
        s0size = savedS0Size;
        spArgsSize = 0;
        savedS0Size = 0;
        isTemp = false;
    }

    RvaMemory* pushArg() {
        occupy(4);
        auto* mem = rvaAlloc.create<RvaMemory>(RvReg::SP, spArgsSize);
        spArgsSize += 4;
        return mem;
    }

    int getSize() const { return std::max(maxSize, s0size + spArgsSize); }

    int getSizeAligned16() const { return ((getSize() + 15) / 16) * 16; }

private:
    RvaMemory* allocate(int bytes) {
        occupy(bytes);
        return rvaAlloc.create<RvaMemory>(RvReg::S0, -s0size);
    }

    std::unordered_map<StringRef, RvaMemory*> var2stack;
    std::unordered_set<StringRef> temporaries;
    Allocator& rvaAlloc;
    int s0size = 0;
    int spArgsSize = 0;
    int maxSize = 0;
    int savedS0Size = 0;
    bool isTemp = false;
};