#pragma once
#include "RvaInstruction.h"
#include "../../../common/Error.h"
#include "../../../skr/instr/SkrBranch.h"

class RvaBranch : public RvaInstruction {
public:
    enum class Operator { Equals, NotEquals, LessThan, LessOrEqual, GreaterThan, GreaterOrEqual };

    RvaBranch(RvaValue* left, Operator op, RvaValue* right, StringRef label) 
        : RvaInstruction(Kind::Branch)
        , left(left)
        , op(op)
        , right(right)
        , label(label) {  }

    void emit(RvListing& listing) override {
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

    static Operator mapOperator(SkrBranch::Operator op) {
        return (RvaBranch::Operator) op; // todo: be careful
    }

    RvaValue* left;
    Operator op;
    RvaValue* right;
    StringRef label;
};
