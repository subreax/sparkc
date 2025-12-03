#include "sparkc/backend/rv/asm/RvListing.h"
#include "Rv32Base.h"
#include "sparkc/common/Error.h"
#include "sparkc/common/LabelGen.h"


RvListing::RvListing(uint8_t* out, size_t cap) : out(out), cap(cap) {  }

void RvListing::add(uint32_t instr) {
    write_u32(instr, offset);
    offset += 4;
}

RvListing& RvListing::operator+=(uint32_t instr) {
    add(instr);
    return *this;
}

void RvListing::addLabel(StringRef label) {
    labels.emplace_back(offset, label);
}

void RvListing::addExternalLabel(StringRef label, void* ptr) {
    size_t offset = ((uint8_t*) ptr) - out;
    labels.emplace_back(offset, label);
}

void RvListing::addWithLabel(uint32_t instr, StringRef label) {
    write_u32(instr, offset);
    unresolved.emplace_back(offset, label);
    offset += 4;
}

void RvListing::link() {
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

size_t RvListing::getSize() const { return offset; }

void RvListing::getPublicLabels(std::vector<Label>& out) {
    for (auto& label : labels) {
        if (LabelGen::isPublic(label.value)) {
            out.emplace_back(label);
        }
    }
}

void RvListing::write_u32(uint32_t instr, int32_t offset) {
    if (offset + 4 <= cap) {
        *((uint32_t*) (out + offset)) = instr;
    } else {
        sparkError("RvListing", "Not enough memory to write compiled program");
    }
}

int32_t RvListing::calculateOffsetToLabel(int32_t pc, StringRef label) {
    return getLabelOffset(label) - pc;
}

int32_t RvListing::getLabelOffset(StringRef label) {
    for (auto& l : labels) {
        if (l.value == label) {
            return l.offset;
        }
    }
    sparkError("RvListing", "Label not found: %s", label);
    return 0;
}
