#pragma once
#include <vector>
#include <cstdint>
#include <cstring>
#include "Rv32Base.h"

class RvListing {
public:
    RvListing(std::vector<uint32_t>& out) : outBin(out) {  }

    void add(uint32_t instr) {
        outBin.emplace_back(instr);
        pc += 4;
    }

    RvListing& operator+=(uint32_t instr) {
        add(instr);
        return *this;
    }

    void addLabel(const char* label) {
        labels.emplace_back(pc, label);
    }

    void addWithLabel(uint32_t instr, const char* label) {
        outBin.emplace_back(instr);
        unresolved.emplace_back(pc / 4, instr, pc, label);
        pc += 4;
    }

    void link() {
        for (Unresolved& u : unresolved) {
            uint32_t instr = u.instr;
            if (Rv32Base::isBType(instr)) {
                instr |= Rv32Base::encodeImmB(calculateOffsetToLabel(u.instrPc, u.label));
            }
            else if (Rv32Base::isJType(instr)) {
                instr |= Rv32Base::encodeImmJ(calculateOffsetToLabel(u.instrPc, u.label));
            }
            else {
                printf("Can't link instruction %08x\n", instr);
                std::abort();
            }
            outBin[u.idx] = instr;
        }
    }

private:
    struct Unresolved {
        Unresolved() = default;
        Unresolved(int32_t idx, int32_t instr, int32_t instrPc, const char* label) 
            : idx(idx)
            , instr(instr)
            , instrPc(instrPc)
            , label(label) {  }

        int32_t idx = 0;
        int32_t instr = 0;
        int32_t instrPc = 0;
        const char* label = nullptr;
    };

    struct Label {
        Label() = default;
        Label(int32_t pc, const char* value) : pc(pc), value(value) {  }

        int32_t pc = 0;
        const char* value = nullptr;
    };

    int32_t calculateOffsetToLabel(int32_t pc, const char* label) {
        return getLabelPos(label) - pc;
    }

    int32_t getLabelPos(const char* label) {
        for (auto& l : labels) {
            if (strncmp(l.value, label, 128) == 0) {
                return l.pc;
            }
        }
        printf("Label not found: %s\n", label);
        std::abort();
        return 0;
    }

    int pc = 0;
    std::vector<uint32_t>& outBin;
    std::vector<Label> labels;
    std::vector<Unresolved> unresolved;
};