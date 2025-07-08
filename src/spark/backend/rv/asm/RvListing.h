#pragma once
#include <vector>
#include <cstdint>
#include <cstring>
#include "Rv32Base.h"
#include "../../../common/Error.h"
#include "../../../common/alloc/NoMemoryException.h"
#include "../../../common/LabelGen.h"

class RvListing {
public:
    struct Label {
        Label() = default;
        Label(int32_t offset, StringRef value) : offset(offset), value(value) {  }

        int32_t offset = 0;
        StringRef value = StringRef::nullInstance();
    };

    RvListing(uint8_t* out, size_t cap) : out(out), cap(cap) {  }

    void add(uint32_t instr) {
        write_u32(instr, offset);
        offset += 4;
    }

    RvListing& operator+=(uint32_t instr) {
        add(instr);
        return *this;
    }

    void addLabel(StringRef label) {
        labels.emplace_back(offset, label);
    }

    void addWithLabel(uint32_t instr, StringRef label) {
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
                sparkError("RvListing", "Can't link instruction: %08x", instr);
            }
            write_u32(instr, u.offset);
        }
    }

    size_t getSize() const { return offset; }

    void getExternalLabels(std::vector<Label>& out) {
        for (auto& label : labels) {
            if (LabelGen::isExternal(label.value)) {
                out.emplace_back(label);
            }
        }
    }

private:
    struct Unresolved {
        Unresolved() = default;
        Unresolved(int32_t offset, StringRef label) 
            : offset(offset)
            , label(label) {  }

        int32_t offset = 0;
        StringRef label = StringRef::nullInstance();
    };

    void write_u32(uint32_t instr, int32_t offset) {
        if (offset + 4 < cap) {
            *((uint32_t*) (out + offset)) = instr;
        } else {
            throw NoMemoryException("Not enough memory to write compiled program");
        }
    }

    int32_t calculateOffsetToLabel(int32_t pc, StringRef label) {
        return getLabelOffset(label) - pc;
    }

    int32_t getLabelOffset(StringRef label) {
        for (auto& l : labels) {
            if (l.value == label) {
                return l.offset;
            }
        }
        sparkError("RvListing", "Label not found: %s", label);
        return 0;
    }

    uint8_t* out;
    size_t cap;
    int32_t offset = 0;
    std::vector<Label> labels;
    std::vector<Unresolved> unresolved;
};