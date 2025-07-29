#pragma once
#include "RvaInstruction.h"

class RvaMov : public RvaInstruction {
public:
    RvaMov(RvaValue* to, RvaValue* from) 
        : RvaInstruction(Kind::Move)
        , to(to)
        , from(from) {}

    void emit(RvListing& listing) override {
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
            } else {
                listing += Rv32I::addi(toReg, RvReg::ZERO, low);
            }
        } else {
            listing += Rv32I::add(toReg, RvReg::ZERO, expectReg(from));
        }
    }

    RvaValue* to;
    RvaValue* from;

private:
    bool isImm11(int32_t imm) {
        return imm >= -2048 && imm <= 2047;
    }

    int32_t lo(int32_t imm) {
        return sext11(imm & 0b111111111111);
    }

    int32_t hi(int32_t imm) {
        return imm >> 12;
    }

    int32_t sext11(int32_t imm11) {
        return imm11 << 20 >> 20;
    }
};
