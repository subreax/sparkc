#pragma once
#include "RvaInstruction.h"
#include "../../../skr/instr/SkrBinary.h"

class RvaBinary : public RvaInstruction {
public:
    enum class Operator {
        Plus,
        Minus,
        Mul,
        Div,
        Rem,
    };

    RvaBinary(RvaValue* dst, RvaValue* left, Operator op, RvaValue* right)
        : RvaInstruction(Type::Binary)
        , dst(dst)
        , left(left)
        , op(op)
        , right(right) {  }

    void emit(RvListing& listing) override {}

    static Operator mapOperator(SkrBinary::Operator op) {
        return (Operator) op; // todo: be careful
    }

    RvaValue* dst;
    RvaValue* left;
    Operator op;
    RvaValue* right;
};
