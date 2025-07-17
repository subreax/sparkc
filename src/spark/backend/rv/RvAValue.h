#pragma once
#include <cstdint>
#include "RvReg.h"
#include "../../common/Constant.h"

class RvaValue {
public:
    enum class Kind { Imm, PseudoReg, PseudoMem, Register, Memory };

    RvaValue(Kind kind) : kind(kind) {  }

    const Kind kind;
};


class RvaImm : public RvaValue {
public:
    RvaImm(int32_t value) : RvaValue(Kind::Imm), value(value) {  }

    int32_t getValue() const { return value; }

private:
    int32_t value;
};

class RvaPseudoReg : public RvaValue {
public:
    RvaPseudoReg(StringRef id) : RvaValue(Kind::PseudoReg), id(id) {  }

    StringRef getId() const { return id; }

private:
    StringRef id;
};

class RvaPseudoMem : public RvaValue {
public:
    RvaPseudoMem(StringRef id, int offset) : RvaValue(Kind::PseudoMem), id(id), offset(offset) {  }

    StringRef getId() const { return id; }
    int getOffset() const { return offset; }

private:
    StringRef id;
    int offset;
};


class RvaRegister : public RvaValue {
public:
    RvaRegister(RvReg reg) : RvaValue(Kind::Register), reg(reg) {  }

    RvReg getReg() const { return reg; }

private:
    RvReg reg;
};


class RvaMemory : public RvaValue {
public:
    RvaMemory(RvReg base, int offset) : RvaValue(Kind::Memory), base(base), offset(offset) {  }

    RvReg getBase() const { return base; }
    int getOffset() const { return offset; }

private:
    RvReg base;
    int offset;
};
