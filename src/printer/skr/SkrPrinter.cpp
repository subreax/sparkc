#include "SkrPrinter.h"
#include "../Colored.h"

std::ostream& operator<<(std::ostream& os, const SkrValue& skr) {
    if (skr.getType() == SkrValue::Type::Const) {
        os << ((SkrConst*) &skr)->getConst();
    } 
    else if (skr.getType() == SkrValue::Type::Var) {
        os << ((SkrVar*) &skr)->getId();
    } 
    else {
        os << "_unknown_: " << (int) skr.getType();
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, SkrBinary::Operator op) {
    static const char* OPS[7] = { "+", "-", "*", "/", "%", "&&", "||" };
    int iop = (int) op;
    if (iop < 7) {
        os << OPS[iop];
    } else {
        os << "_unknown_:" << iop;
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, SkrBranch::Operator op) {
    static const char* OPS[2] = { "==", "!=" };
    int iop = (int) op;
    if (iop < 2) {
        os << OPS[iop];
    } else {
        os << "_unknown_:" << iop;
    }
    return os;
}

void SkrPrinter::print(std::ostream& os, SkrFunction* func) {
    os << "fun " << Colored::label(func->getName()) << "(";
    auto params = func->getParams();
    for (size_t i = 0; i < params.size(); i++) {
        os << *params[i] << ": int";
        if (i != params.size() - 1) {
            os << ", ";
        }
    }
    os << "): int \n";


    const auto& skrs = func->getInstructions();
    for (auto* skr : skrs) {
        os << "    ";
        print(os, skr);
    }
}

void SkrPrinter::print(std::ostream& os, SkrInstruction* skr) {
    auto type = skr->getType();
    if (type == SkrInstruction::Type::Binary) {
        auto* bin = (SkrBinary*) skr;
        os << *bin->getDst() << " = " << *bin->getLeft() << " " << bin->getOperator() << " " << *bin->getRight();
    }
    else if (type == SkrInstruction::Type::Copy) {
        auto* it = (SkrCopy*) skr;
        os << *it->getTo() << " = " << *it->getFrom();
    }
    else if (type == SkrInstruction::Type::Jump) {
        auto* it = (SkrJump*) skr;
        os << "Jump to " << Colored::label(it->getLabel());
    }
    else if (type == SkrInstruction::Type::Label) {
        auto* it = (SkrLabel*) skr;
        os << Colored::label(it->getLabel()) << ":";
    }
    else if (type == SkrInstruction::Type::Branch) {
        auto* it = (SkrBranch*) skr;
        os << "branch to " << Colored::label(it->getLabel()) << " if " << *it->getLeft() << " " << it->getOperator() << " " << *it->getRight();
    }
    else if (type == SkrInstruction::Type::FunCall) {
        auto* it = (SkrFunCall*) skr;
        os << *it->getRetVar() << " = " << Colored::label(it->getName()) << "(";
        auto args = it->getArgs();
        for (size_t i = 0; i < args.size(); i++) {
            os << *args[i];
            if (i != args.size() - 1) {
                os << ", ";
            }
        }
        os << ")";
    }
    else {
        os << "unknown skr: " << (int) type;
    }
    os << "\n";
}