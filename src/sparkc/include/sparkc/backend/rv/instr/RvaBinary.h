#pragma once
#include "RvaInstruction.h"
#include "sparkc/backend/rv/RvaValue.h"
#include "sparkc/skr/instr/SkrBinary.h"

class RvaBinary : public RvaInstruction {
public:
    enum class Operator {
        Plus,
        Minus,
        Mul,
        MulH,
        Div,
        Rem,
        Equals,
        NotEquals,
        LessThan,
        LessOrEqual,
        GreaterThan,
        GreaterOrEqual,
        ShiftLeft,
        ShiftRight,
        Or,
        _FixedMul,
        _FixedDiv
    };

    RvaBinary(RvaValue* dst, RvaValue* left, Operator op, RvaValue* right);

    void emit(RvListing& listing) override;

    bool supportImm();

    static bool hasImmSupport(Operator op);
    static Operator mapOperator(SkrBinary::Operator op);

    RvaValue* dst;
    RvaValue* left;
    Operator op;
    RvaValue* right;
};
