#include "SkrPrinter.h"
#include "../Colored.h"

std::ostream& operator<<(std::ostream& os, const SymbolType& type) {
    os << type.toString();
    return os;
}

std::ostream& operator<<(std::ostream& os, const SkrValue& skr) {
    if (skr.isConst()) {
        auto* c = skr.toSkrConst()->getConst();
        if (c->type->kind == SymbolType::Kind::Integer) {
            os << ((IntConstant*) c)->val;
        }
        else if (c->type->kind == SymbolType::Kind::Float) {
            os << ((FloatConstant*) c)->val;
        }
        else {
            os << "err_unknown_constant";
        }
    }
    else if (skr.isVar()) {
        os << ((SkrVar*) &skr)->getId().toString();
    }
    else {
        os << "_unknown_: " << (int) skr.kind;
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, SkrBinary::Operator op) {
    static constexpr const char* OPS[] = { "+", "-", "*", "/", "%", "==", "!=", "<", "<=", ">", ">=" };
    static constexpr int OPS_SZ = sizeof(OPS) / sizeof(const char*);
    int iop = (int) op;
    if (iop < OPS_SZ) {
        os << OPS[iop];
    }
    else {
        os << "_unknown_:" << iop;
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, SkrBranch::Operator op) {
    static const char* OPS[] = { "==", "!=", "<", "<=", ">", ">=" };
    static constexpr int OPS_SZ = sizeof(OPS) / sizeof(const char*);
    int iop = (int) op;
    if (iop < OPS_SZ) {
        os << OPS[iop];
    }
    else {
        os << "_unknown_:" << iop;
    }
    return os;
}

void SkrPrinter::print(std::ostream& os, SkrFunction* func, const SymbolTable& table) {
    os << "fun " << Colored::label(func->getName()) << "(";
    auto* funcType = (SymbolFunctionType*) table.get(func->getName());
    auto params = func->getParams();
    for (size_t i = 0; i < params.size(); i++) {
        auto* type = table.get(params[i]->getId());
        os << *params[i] << ": " << *type;
        if (i != params.size() - 1) {
            os << ", ";
        }
    }
    os << "): " << *funcType->getReturnType() << "\n";

    const auto& skrs = func->getInstructions();
    for (auto* skr : skrs) {
        os << "    ";
        print(os, skr, table);
        os << "\n";
    }
}

void SkrPrinter::print(
    std::ostream& os,
    SkrInstruction* skr,
    const SymbolTable& table,
    bool colored
) {
    auto kind = skr->kind;
    if (kind == SkrInstruction::Kind::Binary) {
        auto* bin = (SkrBinary*) skr;
        os << *bin->getDst() << " = "
           << *bin->getLeft() << " " << bin->getOperator() << " " << *bin->getRight();
    }
    else if (kind == SkrInstruction::Kind::Copy) {
        auto* it = (SkrCopy*) skr;
        os << "copy " << *it->getTo() << " = " << *it->getFrom();
    }
    else if (kind == SkrInstruction::Kind::Jump) {
        auto* it = (SkrJump*) skr;
        os << "jmp ";
        if (colored) {
            os << Colored::label(it->getLabel());
        }
        else {
            os << it->getLabel().toString();
        }
    }
    else if (kind == SkrInstruction::Kind::Label) {
        auto* it = (SkrLabel*) skr;
        if (colored) {
            os << Colored::label(it->getLabel()) << ":";
        }
        else {
            os << it->getLabel().toString() << ":";
        }
    }
    else if (kind == SkrInstruction::Kind::Branch) {
        auto* it = (SkrBranch*) skr;
        os << "jmp ";
        if (colored) {
            os << Colored::label(it->getLabel());
        }
        else {
            os << it->getLabel().toString();
        }
        os << " if " << *it->getLeft() << " " << it->getOperator() << " " << *it->getRight();
    }
    else if (kind == SkrInstruction::Kind::FunCall) {
        auto* it = (SkrFunCall*) skr;
        os << *it->getRetVar() << " = ";
        if (colored) {
            os << Colored::label(it->getName());
        }
        else {
            os << it->getName().toString();
        }
        os << "(";
        auto args = it->getArgs();
        for (size_t i = 0; i < args.size(); i++) {
            os << *args[i];
            if (i != args.size() - 1) {
                os << ", ";
            }
        }
        os << ")";
    }
    else if (kind == SkrInstruction::Kind::Int2Float) {
        auto* it = (SkrInt2Float*) skr;
        os << *it->getDst() << " = (float) " << *it->getSrc();
    }
    else if (kind == SkrInstruction::Kind::Float2Int) {
        auto* it = (SkrFloat2Int*) skr;
        os << *it->getDst() << " = (int) " << *it->getSrc();
    }
    else if (kind == SkrInstruction::Kind::Load) {
        auto* it = (SkrLoad*) skr;
        os << "load " << *it->getTo() << " = " << it->getFromOffset() << "(" << *it->getFrom() << ")";
    }
    else if (kind == SkrInstruction::Kind::Store) {
        auto* it = (SkrStore*) skr;
        os << "store " << it->getToOffset() << "(" << *it->getTo() << ") = " << *it->getFrom();
    }
    else if (kind == SkrInstruction::Kind::GetAddr) {
        auto* it = (SkrGetAddr*) skr;
        os << *it->getTo() << " = addrOf(" << *it->getVar() << ")";
    }
    else if (kind == SkrInstruction::Kind::CopyToOffset) {
        auto* it = (SkrCopyToOffset*) skr;
        os << "copy " << it->getToOffset() << "(" << *it->getTo() << ") = " << *it->getFrom();
    }
    else if (kind == SkrInstruction::Kind::CopyFromOffset) {
        auto* it = (SkrCopyFromOffset*) skr;
        os << "copy " << *it->getTo() << " = " << it->getFromOffset() << "(" << *it->getFrom()
           << ")";
    }
    else {
        os << "unknown skr: " << (int) kind;
    }
}