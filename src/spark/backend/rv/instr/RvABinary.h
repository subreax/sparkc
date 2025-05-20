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

    void emit(RvListing& listing) override {
        switch (op) {
        case Operator::Plus:
            listing += Rv32I::add(expectReg(dst), expectReg(left), expectReg(right));
            break;

        case Operator::Minus:
            listing += Rv32I::sub(expectReg(dst), expectReg(left), expectReg(right));
            break;
        
        case Operator::Mul:
            listing += Rv32M::mul(expectReg(dst), expectReg(left), expectReg(right));
            break;

        case Operator::Div:
            listing += Rv32M::div(expectReg(dst), expectReg(left), expectReg(right));
            break;

        case Operator::Rem:
            listing += Rv32M::rem(expectReg(dst), expectReg(left), expectReg(right));
            break;

        default:
            printf("Unknown RvaBinary operator: %d\n", op);
            std::abort();
        }
    }

    static Operator mapOperator(SkrBinary::Operator op) {
        return (Operator) op; // todo: be careful
    }

    RvaValue* dst;
    RvaValue* left;
    Operator op;
    RvaValue* right;
};
