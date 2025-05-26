#pragma once
#include "RvaInstruction.h"

class RvaMov : public RvaInstruction {
public:
    RvaMov(RvaValue* to, RvaValue* from) 
        : RvaInstruction(Kind::Move)
        , to(to)
        , from(from) {}

    void emit(RvListing& listing) override {
        if (from->kind == RvaValue::Kind::Imm) {
            int32_t value = ((RvaImm*) from)->getValue();
            listing += Rv32I::addi(expectReg(to), RvReg::ZERO, value); // todo: lui if needed
        } else {
            listing += Rv32I::add(expectReg(to), RvReg::ZERO, expectReg(from));
        }
    }

    RvaValue* to;
    RvaValue* from;
};
