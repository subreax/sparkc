#include "SkrPrinter.h"
#include "../Colored.h"

static const char* op(SkrBinary::Operator op) {
    return SkrBinary::operatorStringValue(op);
}

static const char* op(SkrBranch::Operator op) {
    return SkrBranch::operatorStringValue(op);
}

std::string SkrPrinter::toString(SkrFunction* func) {
    sb << "fun " << label(func->getName()) << "(";

    auto* funcDecl = (SymbolFunctionType*) symbolTable.get(func->getName());
    auto params = func->getParams();
    for (size_t i = 0; i < params.size(); i++) {
        auto* type = symbolTable.get(params[i]->getId());
        sb << val(params[i]);
        if (i != params.size() - 1) {
            sb << ", ";
        }
    }
    sb << "): " << type(funcDecl->getReturnType()) << "\n";

    const auto& skrs = func->getInstructions();
    for (auto* skr : skrs) {
        sb << "    ";
        append(skr);
        sb << "\n";
    }

    return sb.toString();
}

void SkrPrinter::append(const SkrInstruction* skr) {
    auto kind = skr->kind;
    if (kind == SkrInstruction::Kind::Binary) {
        auto* bin = (SkrBinary*) skr;
        sb << val(bin->getDst()) << " = "
           << type(bin->getDst()) << " "
           << val(bin->getLeft())
           << " " << op(bin->getOperator()) << " "
           << val(bin->getRight());
    }
    else if (kind == SkrInstruction::Kind::Copy) {
        auto* it = (SkrCopy*) skr;
        sb << "copy "
           << val(it->getTo()) << " = " << type(it->getFrom()) << " " << val(it->getFrom());
    }
    else if (kind == SkrInstruction::Kind::Jump) {
        auto* it = (SkrJump*) skr;
        sb << "jmp " << label(it->getLabel());
    }
    else if (kind == SkrInstruction::Kind::Label) {
        auto* it = (SkrLabel*) skr;
        sb << label(it->getLabel()) << ":";
    }
    else if (kind == SkrInstruction::Kind::Branch) {
        auto* it = (SkrBranch*) skr;
        sb << "jmp " << label(it->getLabel())
           << " if "
           << type(it->getLeft()) << " " << val(it->getLeft())
           << " " << op(it->getOperator()) << " "
           << val(it->getRight());
    }
    else if (kind == SkrInstruction::Kind::FunCall) {
        auto* it = (SkrFunCall*) skr;
        auto args = it->getArgs();
        sb << val(it->getRetVar()) << " = " << type(it->getRetVar()) << " " << label(it->getName()) << "(";
        for (size_t i = 0; i < args.size(); i++) {
            sb << val(args[i]);
            if (i != args.size() - 1) {
                sb << ", ";
            }
        }
        sb << ")";
    }
    else if (kind == SkrInstruction::Kind::Int2Float) {
        auto* it = (SkrInt2Float*) skr;
        sb << val(it->getDst()) << " = toFloat(" << val(it->getSrc()) << ")";
    }
    else if (kind == SkrInstruction::Kind::Float2Int) {
        auto* it = (SkrFloat2Int*) skr;
        sb << val(it->getDst()) << " = toInt(" << val(it->getSrc()) << ")";
    }
    else if (kind == SkrInstruction::Kind::Load) {
        auto* it = (SkrLoad*) skr;
        sb << "load "
           << val(it->getTo()) << " = " << type(it->getTo()) << it->getFromOffset() << "(" << val(it->getFrom()) << ")";
    }
    else if (kind == SkrInstruction::Kind::Store) {
        auto* it = (SkrStore*) skr;
        sb << "store "
           << it->getToOffset() << "(" << val(it->getTo()) << ") = " << type(it->getFrom()) << " " << val(it->getFrom());
    }
    else if (kind == SkrInstruction::Kind::GetAddr) {
        auto* it = (SkrGetAddr*) skr;
        sb << val(it->getTo()) << " = addrOf(" << val(it->getVar()) << ")";
    }
    else if (kind == SkrInstruction::Kind::CopyToOffset) {
        auto* it = (SkrCopyToOffset*) skr;
        sb << "copy "
           << it->getToOffset() << "(" << val(it->getTo()) << ") = " << type(it->getFrom()) << " " << val(it->getFrom());
    }
    else if (kind == SkrInstruction::Kind::CopyFromOffset) {
        auto* it = (SkrCopyFromOffset*) skr;
        sb << "copy "
           << val(it->getTo()) << " = " << type(it->getTo()) << " " << it->getFromOffset() << "(" << val(it->getFrom()) << ")";
    }
    else {
        sparkError("SkrPrinter", "Unknown skr: %d", kind);
    }
}

std::string SkrPrinter::val(const SkrValue* val) const {
    if (val->isConst()) {
        auto* c = val->toSkrConst()->getConst();
        if (c->type->kind == SymbolType::Kind::Integer) {
            return std::to_string(c->intValue());
        }
        else if (c->type->kind == SymbolType::Kind::Float) {
            return std::to_string(c->floatValue());
        }
        else {
            sparkError("SkrPrinter", "Unknown Constant::Kind %d", c->type->kind);
        }
    }
    else if (val->isVar()) {
        return val->toSkrVar()->getId().toString();
    }
    else {
        sparkError("SkrPrinter", "Unknown SkrValue::Kind %d", val->kind);
    }
    return "";
}

std::string SkrPrinter::type(const SkrValue* val) const {
    if (val->isConst()) {
        auto* skrConst = (const SkrConst*) val;
        auto* constant = skrConst->getConst();
        return type(constant->type);
    }
    else if (val->isVar()) {
        auto* skrVar = (const SkrVar*) val;
        return type(symbolTable.get(skrVar->getId()));
    }
    sparkError("SkrPrinter2", "Unknown SkrValue kind");
    return "";
}

std::string SkrPrinter::type(const SymbolType* t) const {
    auto tstr = t->toString();
    return isColored ? Colored::type(tstr) : tstr;
}

std::string SkrPrinter::label(StringRef value) const {
    if (isColored) {
        return Colored::label(value);
    }
    return value.toString();
}
