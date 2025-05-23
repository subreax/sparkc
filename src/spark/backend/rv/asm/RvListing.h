#pragma once
#include <vector>
#include <cstdint>
#include <cstring>
#include "Rv32Base.h"
#include "../../../common/NoMemoryException.h"

class RvListing {
public:
    RvListing(uint8_t* out, size_t cap) : out(out), cap(cap) {  }

    void add(uint32_t instr) {
        write_u32(instr, offset);
        offset += 4;
    }

    RvListing& operator+=(uint32_t instr) {
        add(instr);
        return *this;
    }

    void addLabel(const char* label) {
        labels.emplace_back(offset, label);
    }

    void addWithLabel(uint32_t instr, const char* label) {
        write_u32(instr, offset);
        unresolved.emplace_back(offset, label);
        offset += 4;
    }

    void link() {
        for (Unresolved& u : unresolved) {
            uint32_t instr = *(uint32_t*) (out + u.offset);
            if (Rv32Base::isBType(instr)) {
                instr |= Rv32Base::encodeImmB(calculateOffsetToLabel(u.offset, u.label));
            }
            else if (Rv32Base::isJType(instr)) {
                instr |= Rv32Base::encodeImmJ(calculateOffsetToLabel(u.offset, u.label));
            }
            else {
                printf("Can't link instruction %08x\n", instr);
                std::abort();
            }
            write_u32(instr, u.offset);
        }
    }

    size_t getSize() const { return offset; }

private:
    struct Unresolved {
        Unresolved() = default;
        Unresolved(int32_t offset, const char* label) 
            : offset(offset)
            , label(label) {  }

        int32_t offset = 0;
        const char* label = nullptr;
    };

    struct Label {
        Label() = default;
        Label(int32_t offset, const char* value) : offset(offset), value(value) {  }

        int32_t offset = 0;
        const char* value = nullptr;
    };

    void write_u32(uint32_t instr, int32_t offset) {
        if (offset + 4 < cap) {
            *((uint32_t*) (out + offset)) = instr;
        } else {
            throw NoMemoryException("Not enough memory to write compiled program");
        }
    }

    int32_t calculateOffsetToLabel(int32_t pc, const char* label) {
        return getLabelOffset(label) - pc;
    }

    int32_t getLabelOffset(const char* label) {
        for (auto& l : labels) {
            if (strncmp(l.value, label, 128) == 0) {
                return l.offset;
            }
        }
        printf("Label not found: %s\n", label);
        std::abort();
        return 0;
    }

    uint8_t* out;
    size_t cap;
    int32_t offset = 0;
    std::vector<Label> labels;
    std::vector<Unresolved> unresolved;
};