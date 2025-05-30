#include "RvaPrinter.h"
#include <iomanip>
#include <cstring>
#include "../Colored.h"

static constexpr const char* _REG_STR[] = {
    "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2", "s0", "s1",
    "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7", "s2", "s3",
    "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s11", "t3", "t4",
    "t5", "t6"
};

static constexpr const char* _RVA_BINARY_OP[] = { "+", "-", "*", "/", "%", "==", "!=", "<", "<=", ">", ">=" };
static constexpr int _RVA_BINARY_OP_SZ = sizeof(_RVA_BINARY_OP) / sizeof(const char*);

static constexpr const char* _RVA_BRANCH_OP[] = { "==", "!=", "<", "<=", ">", ">=" };
static constexpr int _RVA_BRANCH_OP_SZ = sizeof(_RVA_BRANCH_OP) / sizeof(const char*);

inline const char* sign(int v) {
    if (v >= 0) {
        return "+";
    } else {
        return "";
    }
}


std::ostream& operator<<(std::ostream& os, RvReg reg) {
    if ((int) reg < 32) {
        os << _REG_STR[(int) reg];
    } else {
        os << "unknown_reg";
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, RvaBinary::Operator op) {
    if ((int) op < _RVA_BINARY_OP_SZ) {
        os << _RVA_BINARY_OP[(int) op];
    } else {
        os << "unknown_op_" << (int) op;
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, RvaBranch::Operator op) {
    if ((int) op < _RVA_BRANCH_OP_SZ) {
        os << _RVA_BRANCH_OP[(int) op];
    } else {
        os << "unknown_op_" << (int) op;
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const RvaValue& value) {
    switch (value.kind) {
    case RvaValue::Kind::Imm:
        os << ((const RvaImm*) &value)->getValue();
        break;

    case RvaValue::Kind::PseudoReg:
        os << "p(" << ((const RvaPseudoReg*) &value)->getId() << ")";
        break;
    
    case RvaValue::Kind::Register:
        os << ((const RvaRegister*) &value)->getReg();
        break;

    case RvaValue::Kind::Memory: {
        auto* it = (const RvaMemory*) &value;
        os << "[" << it->getBase() << sign(it->getOffset()) << it->getOffset() << "]";
    }
        break;

    default:
        sparkError("RvaPrinter", "Unknown RvaValue");
        break;
    }
    return os;
}


void printType(std::ostream& os, const char* type) {
    char buf[16];
    snprintf(buf, 16, "%-15s", type);
    os << buf;
}

void printBinary(std::ostream& os, const RvaBinary* it) {
    printType(os, "binary");
    os << *it->dst << " = " << *it->left << " " << it->op << " " << *it->right;
}

void printMove(std::ostream& os, const RvaMov* it) {
    printType(os, "move");
    os << *it->to << " = " << *it->from;
}

void printLabel(std::ostream& os, const RvaLabel* it) {
    printType(os, "label");
    os << Colored::label(it->getValue()) << ":";
}

void printJump(std::ostream& os, const RvaJump* it) {
    printType(os, "jump");
    os << "jump to " << Colored::label(it->getLabel());
}

void printLoad(std::ostream& os, const RvaLoad* it) {
    printType(os, "load");
    os << *it->fromAddr << " --> " << *it->to;
}

void printStore(std::ostream& os, const RvaStore* it) {
    printType(os, "store");
    os << *it->from << " --> " << *it->toAddr;
}

void printRet(std::ostream& os, const RvaRet* it) {
    printType(os, "return");
    os << "ret";
}

void printPrologue(std::ostream& os, const RvaPrologue* it) {
    printType(os, "prologue");
    os << "prologue " << it->getFrameSize();
    if (it->willSaveRa()) {
        os << " (+RA)";
    }
}

void printEpilogue(std::ostream& os, const RvaEpilogue* it) {
    printType(os, "epilogue");
    os << "epilogue " << it->getFrameSize();
    if (it->willLoadRa()) {
        os << " (+RA)";
    }
}

void printBranch(std::ostream& os, const RvaBranch* it) {
    printType(os, "branch");
    os << "branch to " << Colored::label(it->label) << " if " << *it->left << " " << it->op << " " << *it->right;
}

void printCall(std::ostream& os, const RvaCall* it) {
    printType(os, "call");
    os << "call " << Colored::label(it->getFunName());
}

void RvaPrinter::print(std::ostream& os, const std::vector<RvaInstruction*>& instructions) {
    for (const auto* instr : instructions) {
        switch (instr->kind) {
        case RvaInstruction::Kind::Binary:
            printBinary(os, (const RvaBinary*) instr);
            break;
        
        case RvaInstruction::Kind::Move:
            printMove(os, (const RvaMov*) instr);
            break;

        case RvaInstruction::Kind::Label:
            printLabel(os, (const RvaLabel*) instr);
            break;

        case RvaInstruction::Kind::Jump:
            printJump(os, (const RvaJump*) instr);
            break;

        case RvaInstruction::Kind::Load:
            printLoad(os, (const RvaLoad*) instr);
            break;

        case RvaInstruction::Kind::Store:
            printStore(os, (const RvaStore*) instr);
            break;

        case RvaInstruction::Kind::Ret:
            printRet(os, (const RvaRet*) instr);
            break;

        case RvaInstruction::Kind::Prologue:
            printPrologue(os, (const RvaPrologue*) instr);
            break;
        
        case RvaInstruction::Kind::Epilogue:
            printEpilogue(os, (const RvaEpilogue*) instr);
            break;

        case RvaInstruction::Kind::Branch:
            printBranch(os, (const RvaBranch*) instr);
            break;

        case RvaInstruction::Kind::Call:
            printCall(os, (const RvaCall*) instr);
            break;

        default: os << "unknown rva kind: " << (int) instr->kind;
        }
        os << "\n";
    }
}
