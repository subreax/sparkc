#pragma once
#include "RvReg.h"
#include "sparkc/common/Constant.h"
#include <cstdint>

class RvaValue {
public:
    enum class Kind {
        Imm,
        PseudoReg,
        PseudoMem,
        Register,
        Memory
    };

    RvaValue(Kind kind)
        : kind(kind) { }

    const Kind kind;
};

class RvaImm : public RvaValue {
public:
    RvaImm(int32_t value)
        : RvaValue(Kind::Imm)
        , value(value) { }

    int32_t getValue() const { return value; }

private:
    int32_t value;
};

class RvaPseudoReg : public RvaValue {
public:
    RvaPseudoReg(StringRef id)
        : RvaValue(Kind::PseudoReg)
        , id(id) { }

    StringRef getId() const { return id; }

private:
    StringRef id;
};

class RvaPseudoMem : public RvaValue {
public:
    RvaPseudoMem(StringRef id, int offset)
        : RvaValue(Kind::PseudoMem)
        , id(id)
        , offset(offset) { }

    StringRef getId() const { return id; }
    int getOffset() const { return offset; }

private:
    StringRef id;
    int offset;
};

class RvaRegister : public RvaValue {
public:
    RvaRegister(RvReg reg)
        : RvaValue(Kind::Register)
        , reg(reg) { }

    RvReg getReg() const { return reg; }

    static RvaRegister* get(RvReg reg) {
        static RvaRegister regs[32] = {
            RvaRegister(RvReg::ZERO),
            RvaRegister(RvReg::RA),
            RvaRegister(RvReg::SP),
            RvaRegister(RvReg::GP),
            RvaRegister(RvReg::TP),
            RvaRegister(RvReg::T0),
            RvaRegister(RvReg::T1),
            RvaRegister(RvReg::T2),
            RvaRegister(RvReg::S0),
            RvaRegister(RvReg::S1),
            RvaRegister(RvReg::A0),
            RvaRegister(RvReg::A1),
            RvaRegister(RvReg::A2),
            RvaRegister(RvReg::A3),
            RvaRegister(RvReg::A4),
            RvaRegister(RvReg::A5),
            RvaRegister(RvReg::A6),
            RvaRegister(RvReg::A7),
            RvaRegister(RvReg::S2),
            RvaRegister(RvReg::S3),
            RvaRegister(RvReg::S4),
            RvaRegister(RvReg::S5),
            RvaRegister(RvReg::S6),
            RvaRegister(RvReg::S7),
            RvaRegister(RvReg::S8),
            RvaRegister(RvReg::S9),
            RvaRegister(RvReg::S10),
            RvaRegister(RvReg::S11),
            RvaRegister(RvReg::T3),
            RvaRegister(RvReg::T4),
            RvaRegister(RvReg::T5),
            RvaRegister(RvReg::T6)
        };

        return &regs[(int) reg];
    }

private:
    RvReg reg;
};

class RvaMemory : public RvaValue {
public:
    RvaMemory(RvReg base, int offset)
        : RvaValue(Kind::Memory)
        , base(base)
        , offset(offset) { }

    RvReg getBase() const { return base; }
    int getOffset() const { return offset; }

private:
    RvReg base;
    int offset;
};
