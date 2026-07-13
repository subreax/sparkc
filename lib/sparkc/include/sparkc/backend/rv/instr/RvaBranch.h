#pragma once
#include "RvaInstruction.h"
#include "sparkc/skr/instr/SkrBranch.h"

class RvaBranch : public RvaInstruction {
public:
    enum class Operator {
        Equals,
        NotEquals,
        LessThan,
        LessOrEqual,
        GreaterThan,
        GreaterOrEqual
    };

    RvaBranch(RvaValue* left, Operator op, RvaValue* right, StringRef label);

    void emit(RvListing& listing) override;

    static Operator mapOperator(SkrBranch::Operator op);

    RvaValue* left;
    Operator op;
    RvaValue* right;
    StringRef label;
};
