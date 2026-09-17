#include "RvaPrinter.h"
#include "sparkc/common/printer/Colored.h"
#include <cstdio>

namespace {

// clang-format off
static constexpr const char* REGISTER_NAMES[] = {
    "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2", "s0", "s1",
    "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7", "s2", "s3",
    "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s11", "t3", "t4",
    "t5", "t6"
};
// clang-format on

static constexpr const char* BINARY_OPERATORS[] = {
    "+",
    "-",
    "*",
    "mulh",
    "/",
    "%",
    "==",
    "!=",
    "<",
    "<=",
    ">",
    ">=",
    "<<",
    ">>",
    "|",
    "fixmul"
};
static constexpr int BINARY_OPERATOR_COUNT = sizeof(BINARY_OPERATORS) / sizeof(const char*);

static constexpr const char* BRANCH_OPERATORS[] = {
    "==",
    "!=",
    "<",
    "<=",
    ">",
    ">="
};
static constexpr int BRANCH_OPERATOR_COUNT = sizeof(BRANCH_OPERATORS) / sizeof(const char*);

static inline const char* sign(int v) {
    if (v >= 0) {
        return "+";
    }
    else {
        return "";
    }
}

static std::ostream& operator<<(std::ostream& os, RvReg reg) {
    if ((int) reg < 32) {
        os << REGISTER_NAMES[(int) reg];
    }
    else {
        os << "unknown_reg";
    }
    return os;
}

static std::ostream& operator<<(std::ostream& os, RvaBinary::Operator op) {
    if ((int) op < BINARY_OPERATOR_COUNT) {
        os << BINARY_OPERATORS[(int) op];
    }
    else {
        os << "unknown_op_" << (int) op;
    }
    return os;
}

static std::ostream& operator<<(std::ostream& os, RvaBranch::Operator op) {
    if ((int) op < BRANCH_OPERATOR_COUNT) {
        os << BRANCH_OPERATORS[(int) op];
    }
    else {
        os << "unknown_op_" << (int) op;
    }
    return os;
}

} // namespace

void RvaPrinter::PrintableValue::print(std::ostream& os) const {
    switch (value.kind) {
    case RvaValue::Kind::Imm:
        os << static_cast<const RvaImm&>(value).getValue();
        break;

    case RvaValue::Kind::PseudoReg:
        os << "pr(" << static_cast<const RvaPseudoReg&>(value).getId().toString() << ")";
        break;

    case RvaValue::Kind::PseudoMem: {
        auto* it = static_cast<const RvaPseudoMem*>(&value);
        os << "pm(" << it->getId().toString() << sign(it->getOffset()) << it->getOffset() << ")";
    } break;

    case RvaValue::Kind::Register:
        os << static_cast<const RvaRegister&>(value).getReg();
        break;

    case RvaValue::Kind::Memory: {
        auto* it = static_cast<const RvaMemory*>(&value);
        os << "[" << it->getBase() << sign(it->getOffset()) << it->getOffset() << "]";
    } break;

    case RvaValue::Kind::Data: {
        auto* it = static_cast<const RvaData*>(&value);
        os << "dm(" << printer.label(it->getLabel()) << sign(it->getOffset()) << it->getOffset() << ")";
    } break;

    default:
        sparkError("RvaPrinter", "Unknown RvaValue");
        break;
    }
}

void RvaPrinter::printType(const char* type) {
    char buf[16];
    std::snprintf(buf, sizeof(buf), "%-15s", type);
    os << buf;
}

std::string RvaPrinter::label(StringRef value) const {
    return isColored ? Colored::label(value) : value.toString();
}

std::string RvaPrinter::comment(const std::string& value) const {
    return isColored ? Colored::comment(value) : value;
}

RvaPrinter& RvaPrinter::append(const std::vector<RvaInstruction*>& instructions) {
    for (const auto* instr : instructions) {
        append(instr);
    }
    return *this;
}

RvaPrinter& RvaPrinter::append(const RvaInstruction* instr) {
    switch (instr->kind) {
    case RvaInstruction::Kind::Binary: {
        auto* it = static_cast<const RvaBinary*>(instr);
        printType("binary");
        os << val(it->dst) << " = " << val(it->left) << " " << it->op << " " << val(it->right);
    } break;

    case RvaInstruction::Kind::Move: {
        auto* it = static_cast<const RvaMov*>(instr);
        printType("move");
        os << val(it->to) << " = " << val(it->from);
    } break;

    case RvaInstruction::Kind::Label: {
        auto* it = static_cast<const RvaLabel*>(instr);
        printType("label");
        os << label(it->getValue()) << ":";
    } break;

    case RvaInstruction::Kind::Jump: {
        auto* it = static_cast<const RvaJump*>(instr);
        printType("jump");
        os << "jump to " << label(it->getLabel());
    } break;

    case RvaInstruction::Kind::Load: {
        auto* it = static_cast<const RvaLoad*>(instr);
        printType("load");
        os << val(it->from) << " --> " << val(it->to);
    } break;

    case RvaInstruction::Kind::Store: {
        auto* it = static_cast<const RvaStore*>(instr);
        printType("store");
        os << val(it->from) << " --> " << val(it->to);
    } break;

    case RvaInstruction::Kind::Ret: {
        printType("return");
        os << "ret";
    } break;

    case RvaInstruction::Kind::Prologue: {
        auto* it = static_cast<const RvaPrologue*>(instr);
        printType("prologue");
        os << "prologue " << it->getFrameSize();
        if (it->willSaveRa()) {
            os << " (+RA)";
        }
    } break;

    case RvaInstruction::Kind::Epilogue: {
        auto* it = static_cast<const RvaEpilogue*>(instr);
        printType("epilogue");
        os << "epilogue " << it->getFrameSize();
        if (it->willLoadRa()) {
            os << " (+RA)";
        }
    } break;

    case RvaInstruction::Kind::Branch: {
        auto* it = static_cast<const RvaBranch*>(instr);
        printType("branch");
        os << "branch to " << label(it->label) << " if " << val(it->left) << " " << it->op << " " << val(it->right);
    } break;

    case RvaInstruction::Kind::Call: {
        auto* it = static_cast<const RvaCall*>(instr);
        printType("call");
        os << "call " << label(it->getFunName());
    } break;

    case RvaInstruction::Kind::GetAddress: {
        auto* it = static_cast<const RvaGetAddress*>(instr);
        printType("get_addr");
        os << val(it->to) << " = addrOf(" << val(it->of) << ")";
    } break;

    case RvaInstruction::Kind::DataAlloc: {
        auto* it = static_cast<const RvaDataAlloc*>(instr);
        printType("data_alloc");
        os << label(it->getLabel()) << " " << it->getSize() << " bytes";
    } break;

    case RvaInstruction::Kind::DLoad: {
        auto* it = static_cast<const RvaDLoad*>(instr);
        printType("dload");
        os << val(it->getSrc()) << " --> " << val(it->getDst()) << " (offsetReg: " << val(it->getTempOffsetReg()) << ")";
    } break;

    case RvaInstruction::Kind::DStore: {
        auto* it = static_cast<const RvaDStore*>(instr);
        printType("dstore");
        os << val(it->getSrc()) << " --> " << val(it->getDst()) << " (offsetReg: " << val(it->getTempOffsetReg()) << ")";
    } break;

    case RvaInstruction::Kind::BeginTempStack: {
        printType("comment");
        os << comment("begin temp stack");
    } break;

    case RvaInstruction::Kind::EndTempStack: {
        printType("comment");
        os << comment("end temp stack");
    } break;

    case RvaInstruction::Kind::ReserveOnStack: {
        auto* it = static_cast<const RvaReserveOnStack*>(instr);
        printType("reserve");
        os << "reserve " << val(it->mem);
    } break;

    default:
        os << "unknown rva kind: " << (int) instr->kind;
    }
    os << "\n";
    return *this;
}
