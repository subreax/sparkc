#include "RvaPrinter.h"

static constexpr const char* _REG_STR[] = {
    "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2", "s0", "s1",
    "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7", "s2", "s3",
    "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s11", "t3", "t4",
    "t5", "t6"
};

static constexpr const char* _RVA_BINARY_OP[] = { "+", "-", "*", "/", "%" };
static constexpr int _RVA_BINARY_OP_SZ = sizeof(_RVA_BINARY_OP) / sizeof(const char*);

inline const char* sign(int v) {
    if (v >= 0) {
        return "+";
    } else {
        return "";
    }
}

std::ostream& operator<<(std::ostream& os, const RvAValue& value) {
    auto type = value.getType();
    switch (type)
    {
    case RvAValue::Type::Imm:
        os << ((const RvAImm*) &value)->getValue();
        break;

    case RvAValue::Type::PseudoReg:
        os << "p(" << ((const RvAPseudoReg*) &value)->getId() << ")";
        break;
    
    case RvAValue::Type::Register:
        os << ((const RvARegister*) &value)->getReg();
        break;

    case RvAValue::Type::Memory: {
        auto* it = (const RvAMemory*) &value;
        os << "*(" << it->getBase() << sign(it->getOffset()) << it->getOffset() << ")";
    }
        break;

    default:
        break;
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, RvReg reg) {
    if ((int) reg < 32) {
        os << _REG_STR[(int) reg];
    } else {
        os << "unknown_reg";
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, RvABinary::Operator op) {
    if ((int) op < _RVA_BINARY_OP_SZ) {
        os << _RVA_BINARY_OP[(int) op];
    } else {
        os << "unknown_op_" << (int) op;
    }
    return os;
}