#include "SkrPrinter.h"

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
    static const char* OPS[5] = { "+", "-", "*", "/", "%" };
    int iop = (int) op;
    if (iop < 5) {
        os << OPS[iop];
    } else {
        os << "_unknown_:" << iop;
    }
    return os;
}

void SkrPrinter::print(std::ostream& os, const std::vector<SkrInstruction*>& skrs) {
    for (auto* skr : skrs) {
        if (skr->getType() == SkrInstruction::Type::Binary) {
            auto* bin = (SkrBinary*) skr;
            os << *bin->getDst() << " = " << *bin->getLeft() << " " << bin->getOperator() << " " << *bin->getRight() << "\n";
        }
        else {
            os << "unknown skr: " << (int) skr->getType() << "\n";
        }
    }
}