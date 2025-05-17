#pragma once
#include <cstdint>
#include "RvReg.h"

class RvAValue {
public:
    enum class Type { Imm, PseudoReg, Register, Memory };

    RvAValue(Type type) : type(type) {  }
    Type getType() const { return type; }

private:
    Type type;
};


class RvAImm : public RvAValue {
public:
    RvAImm(int32_t value) : RvAValue(Type::Imm), value(value) {  }

    int32_t getValue() const { return value; }

private:
    int32_t value;
};


class RvAPseudoReg : public RvAValue {
public:
    RvAPseudoReg(const char* id) : RvAValue(Type::PseudoReg), id(id) {  }

    const char* getId() const { return id; }

private:
    const char* id;
};


class RvARegister : public RvAValue {
public:
    RvARegister(RvReg reg) : RvAValue(Type::Register), reg(reg) {  }

    RvReg getReg() const { return reg; }

private:
    RvReg reg;
};


class RvAMemory : public RvAValue {
public:
    RvAMemory(RvReg base, int offset) : RvAValue(Type::Memory), base(base), offset(offset) {  }

    RvReg getBase() const { return base; }
    int getOffset() const { return offset; }

private:
    RvReg base;
    int offset;
};
