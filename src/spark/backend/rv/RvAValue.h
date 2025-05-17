#pragma once
#include <cstdint>
#include "RvReg.h"

class RvaValue {
public:
    enum class Type { Imm, PseudoReg, Register, Memory };

    RvaValue(Type type) : type(type) {  }
    Type getType() const { return type; }

private:
    Type type;
};


class RvaImm : public RvaValue {
public:
    RvaImm(int32_t value) : RvaValue(Type::Imm), value(value) {  }

    int32_t getValue() const { return value; }

private:
    int32_t value;
};


class RvaPseudoReg : public RvaValue {
public:
    RvaPseudoReg(const char* id) : RvaValue(Type::PseudoReg), id(id) {  }

    const char* getId() const { return id; }

private:
    const char* id;
};


class RvaRegister : public RvaValue {
public:
    RvaRegister(RvReg reg) : RvaValue(Type::Register), reg(reg) {  }

    RvReg getReg() const { return reg; }

private:
    RvReg reg;
};


class RvaMemory : public RvaValue {
public:
    RvaMemory(RvReg base, int offset) : RvaValue(Type::Memory), base(base), offset(offset) {  }

    RvReg getBase() const { return base; }
    int getOffset() const { return offset; }

private:
    RvReg base;
    int offset;
};
