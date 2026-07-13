#include "sparkc/backend/rv/instr/RvaMov.h"
#include "../asm/Rv32I.h"

RvaMov::RvaMov(RvaValue* to, RvaValue* from)
    : RvaInstruction(Kind::Move)
    , to(to)
    , from(from) { }

void RvaMov::emit(RvListing& listing) {
    auto toReg = expectReg(to);

    if (from->kind == RvaValue::Kind::Imm) {
        int32_t value = ((RvaImm*) from)->getValue();
        int32_t low = lo(value);
        int32_t high = hi(value);
        if (low < 0) {
            high += 1;
        }

        if (high != 0) {
            listing += Rv32I::lui(toReg, high);
            if (low != 0) {
                listing += Rv32I::addi(toReg, toReg, low);
            }
        }
        else {
            listing += Rv32I::addi(toReg, RvReg::ZERO, low);
        }
    }
    else {
        listing += Rv32I::add(toReg, RvReg::ZERO, expectReg(from));
    }
}

bool RvaMov::isImm11(int32_t imm) { return imm >= -2048 && imm <= 2047; }

int32_t RvaMov::lo(int32_t imm) { return sext11(imm & 0b111111111111); }

int32_t RvaMov::hi(int32_t imm) { return imm >> 12; }

int32_t RvaMov::sext11(int32_t imm11) { return imm11 << 20 >> 20; }
