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
        auto split = Rv32Base::splitImm11(value);

        if (split.hi != 0) {
            listing += Rv32I::lui(toReg, split.hi);
            if (split.lo != 0) {
                listing += Rv32I::addi(toReg, toReg, split.lo);
            }
        }
        else {
            listing += Rv32I::addi(toReg, RvReg::ZERO, split.lo);
        }
    }
    else {
        listing += Rv32I::add(toReg, RvReg::ZERO, expectReg(from));
    }
}
