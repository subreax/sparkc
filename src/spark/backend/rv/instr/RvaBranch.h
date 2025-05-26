#pragma once
#include "RvaInstruction.h"
#include "../../../common/Error.h"
#include "../../../skr/instr/SkrBranch.h"

class RvaBranch : public RvaInstruction {
public:
    enum class Operator { Equals, NotEquals };

    RvaBranch(RvaValue* left, Operator op, RvaValue* right, const char* label) 
        : RvaInstruction(Kind::Branch)
        , left(left)
        , op(op)
        , right(right)
        , label(label) {  }

    void emit(RvListing& listing) override {
        if (op == Operator::Equals) {
            listing.addWithLabel(Rv32I::beq(expectReg(left), expectReg(right)), label);
        }
        else if (op == Operator::NotEquals) {
            listing.addWithLabel(Rv32I::bne(expectReg(left), expectReg(right)), label);
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
    const char* label;
};
