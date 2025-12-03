#include "sparkc/backend/rv/instr/RvaBranch.h"
#include "../asm/Rv32I.h"
#include "sparkc/common/Error.h"

RvaBranch::RvaBranch(
    RvaValue* left, Operator op, RvaValue* right, StringRef label)
    : RvaInstruction(Kind::Branch), left(left), op(op), right(right),
      label(label) {}

void RvaBranch::emit(RvListing& listing) {
    auto L = expectReg(left);
    auto R = expectReg(right);
    if (op == Operator::Equals) {
        listing.addWithLabel(Rv32I::beq(L, R), label);
    }
    else if (op == Operator::NotEquals) {
        listing.addWithLabel(Rv32I::bne(L, R), label);
    }
    else if (op == Operator::LessThan) {
        listing.addWithLabel(Rv32I::blt(L, R), label);
    }
    else if (op == Operator::LessOrEqual) {
        listing.addWithLabel(Rv32I::bge(R, L), label);
    }
    else if (op == Operator::GreaterThan) {
        listing.addWithLabel(Rv32I::blt(R, L), label);
    }
    else if (op == Operator::GreaterOrEqual) {
        listing.addWithLabel(Rv32I::bge(L, R), label);
    }
    else {
        sparkError("RvaBranch", "Unknown operator: %d", op);
    }
}

RvaBranch::Operator RvaBranch::mapOperator(SkrBranch::Operator op) {
    return (RvaBranch::Operator) op; // todo: be careful
}
