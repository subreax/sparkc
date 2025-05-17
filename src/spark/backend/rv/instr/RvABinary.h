#pragma once
#include "RvAInstruction.h"
#include "../../../skr/instr/SkrBinary.h"

class RvABinary : public RvAInstruction {
public:
    enum class Operator {
        Plus,
        Minus,
        Mul,
        Div,
        Rem,
    };

    RvABinary(RvAValue* dst, RvAValue* left, Operator op, RvAValue* right)
        : RvAInstruction(Type::Binary)
        , dst(dst)
        , left(left)
        , op(op)
        , right(right) {  }

    void emit(RvListing& listing) override {}

    static Operator mapOperator(SkrBinary::Operator op) {
        return (Operator) op; // todo: be careful
    }

    RvAValue* dst;
    RvAValue* left;
    Operator op;
    RvAValue* right;
};
