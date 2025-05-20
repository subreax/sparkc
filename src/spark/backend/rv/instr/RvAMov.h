#pragma once
#include "RvaInstruction.h"

class RvaMov : public RvaInstruction {
public:
    RvaMov(RvaValue* to, RvaValue* from) 
        : RvaInstruction(Type::Move)
        , to(to)
        , from(from) {}

    void emit(RvListing& listing) override {
        if (from->getType() == RvaValue::Type::Imm) {
            int32_t value = ((RvaImm*) from)->getValue();
            listing += Rv32I::addi(expectReg(to), RvReg::ZERO, value); // todo: lui if needed
        } else {
            listing += Rv32I::add(expectReg(to), RvReg::ZERO, expectReg(from));
        }
    }

    RvaValue* to;
    RvaValue* from;
};
