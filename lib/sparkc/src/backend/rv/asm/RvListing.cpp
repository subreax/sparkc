#include "sparkc/backend/rv/asm/RvListing.h"
#include "Rv32I.h"
#include "sparkc/common/Error.h"
#include "sparkc/common/LabelGen.h"

RvListing::RvListing(uint8_t* out, size_t cap)
    : out(out)
    , cap(cap) { }

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
    size_t offset = ((uint8_t*) ptr) - out; /* todo: is size_t a typo? */
    labels.emplace_back(offset, label);
}

/* todo: clarify function name. addLabel function adds label to labels, but this adds to unresolved. */
void RvListing::addWithLabel(uint32_t instr, StringRef label) {
    write_u32(instr, offset);
    unresolved.emplace_back(offset, label);
    offset += 4;
}

void RvListing::link() {
    for (Unresolved& u : unresolved) {
        uint32_t instr = get_u32(u.offset);
        if (Rv32I::isBranch(instr)) {
            // todo: check that imm value fits in the instruction
            instr |= Rv32Base::encodeImmB(calculateOffsetToLabel(u.offset, u.label));
            write_u32(instr, u.offset);
        }
        else if (Rv32I::isJal(instr)) {
            // todo: check that imm value fits in the instruction
            instr |= Rv32Base::encodeImmJ(calculateOffsetToLabel(u.offset, u.label));
            write_u32(instr, u.offset);
        }
        else if (Rv32I::isAuipc(instr) && Rv32I::isJalr(get_u32(u.offset + 4))) {
            int32_t funOffset = calculateOffsetToLabel(u.offset, u.label);
            if (Rv32Base::isImm20(funOffset - 4)) {
                write_u32(Rv32I::nop(), u.offset);
                write_u32(Rv32I::jal(RvReg::RA, funOffset - 4), u.offset + 4);
            }
            else {
                RvReg rd = Rv32Base::uTypeReadRd(instr);
                auto split = Rv32Base::splitImm11(funOffset);
                write_u32(Rv32I::auipc(rd, split.hi), u.offset);
                write_u32(Rv32I::jalr(RvReg::RA, rd, split.lo), u.offset + 4);
            }
        }
        else {
            sparkError("RvListing", "Can't link instruction: %08x", instr);
        }
    }
}

size_t RvListing::getSize() const { return offset; }

std::vector<Label> RvListing::getPublicLabels() const {
    std::vector<Label> outLabels;
    for (auto& label : labels) {
        if (!isLabelExternal(label) && LabelGen::isPublic(label.value)) {
            outLabels.emplace_back(label);
        }
    }
    return outLabels;
}

void RvListing::write_u32(uint32_t instr, int32_t offset) {
    if (offset + 4 <= cap) {
        *((uint32_t*) (out + offset)) = instr;
    }
    else {
        sparkError("RvListing", "Not enough memory to write compiled program");
    }
}

uint32_t& RvListing::get_u32(uint32_t offset) {
    static uint32_t sNullValue = 0;

    if (offset + 4 <= cap) {
        return *(uint32_t*) (out + offset);
    }
    else {
        sparkError("RvListing", "get_u32 is out of range");
        return sNullValue;
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

bool RvListing::isLabelExternal(const Label& label) const {
    return label.offset < 0 || label.offset >= cap;
}