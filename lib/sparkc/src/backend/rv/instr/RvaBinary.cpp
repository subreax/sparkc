#include "sparkc/backend/rv/instr/RvaBinary.h"
#include "../asm/Rv32I.h"
#include "../asm/Rv32M.h"
#include "sparkc/common/Error.h"

RvaBinary::RvaBinary(
    RvaValue* dst,
    RvaValue* left,
    Operator op,
    RvaValue* right
)
    : RvaInstruction(Kind::Binary)
    , dst(dst)
    , left(left)
    , op(op)
    , right(right) {
}

void RvaBinary::emit(RvListing& listing) {
    switch (op) {
    case Operator::Plus:
        // todo: support left or right as imm
        if (right->kind == RvaValue::Kind::Imm) {
            listing += Rv32I::addi(expectReg(dst), expectReg(left), expectImm(right));
        }
        else {
            listing += Rv32I::add(expectReg(dst), expectReg(left), expectReg(right));
        }
        break;

    case Operator::Minus:
        // todo: support left or right as imm
        listing += Rv32I::sub(expectReg(dst), expectReg(left), expectReg(right));
        break;

    case Operator::Mul:
        listing += Rv32M::mul(expectReg(dst), expectReg(left), expectReg(right));
        break;

    case Operator::MulH:
        listing += Rv32M::mulh(expectReg(dst), expectReg(left), expectReg(right));
        break;

    case Operator::Div:
        listing += Rv32M::div(expectReg(dst), expectReg(left), expectReg(right));
        break;

    case Operator::Rem:
        listing += Rv32M::rem(expectReg(dst), expectReg(left), expectReg(right));
        break;

    case Operator::Equals:
        // todo: support left or right as imm
        listing += Rv32I::sub(expectReg(dst), expectReg(left), expectReg(right));
        listing += Rv32I::seqz(expectReg(dst), expectReg(dst));
        break;

    case Operator::NotEquals:
        listing += Rv32I::sub(expectReg(dst), expectReg(left), expectReg(right));
        listing += Rv32I::snez(expectReg(dst), expectReg(dst));
        break;

    case Operator::LessThan:
        // todo: support left or right as imm
        listing += Rv32I::slt(expectReg(dst), expectReg(left), expectReg(right));
        break;

    case Operator::LessOrEqual:
        // todo: support left or right as imm

        // (left > right) == 0
        listing += Rv32I::sgt(expectReg(dst), expectReg(left), expectReg(right));
        listing += Rv32I::seqz(expectReg(dst), expectReg(dst));
        break;

    case Operator::GreaterThan:
        // todo: support left or right as imm
        listing += Rv32I::sgt(expectReg(dst), expectReg(left), expectReg(right));
        break;

    case Operator::GreaterOrEqual:
        // todo: support left or right as imm

        // (left < right) == 0
        listing += Rv32I::slt(expectReg(dst), expectReg(left), expectReg(right));
        listing += Rv32I::seqz(expectReg(dst), expectReg(dst));
        break;

    case Operator::ShiftLeft:
        listing += Rv32I::slli(expectReg(dst), expectReg(left), expectImm(right));
        break;

    case Operator::ShiftRight:
        listing += Rv32I::srli(expectReg(dst), expectReg(left), expectImm(right));
        break;

    case Operator::Or:
        if (right->kind == RvaValue::Kind::Imm) {
            listing += Rv32I::ori(expectReg(dst), expectReg(left), expectImm(right));
        }
        else {
            listing += Rv32I::or_(expectReg(dst), expectReg(left), expectReg(right));
        }
        break;

    default: sparkError("RvaBinary", "Unknown operator: %d", op);
    }
}

bool RvaBinary::supportImm() { return hasImmSupport(op); }

bool RvaBinary::hasImmSupport(Operator op) {
    switch (op) {
    case Operator::ShiftLeft:
    case Operator::ShiftRight:
    case Operator::Or: return true;
    }

    return false;
}

RvaBinary::Operator RvaBinary::mapOperator(SkrBinary::Operator op) {
    switch (op) {
    case SkrBinary::Operator::Plus: return Operator::Plus;
    case SkrBinary::Operator::Minus: return Operator::Minus;
    case SkrBinary::Operator::Mul: return Operator::Mul;
    case SkrBinary::Operator::Div: return Operator::Div;
    case SkrBinary::Operator::Rem: return Operator::Rem;
    case SkrBinary::Operator::Equals: return Operator::Equals;
    case SkrBinary::Operator::NotEquals: return Operator::NotEquals;
    case SkrBinary::Operator::LessThan: return Operator::LessThan;
    case SkrBinary::Operator::LessOrEqual: return Operator::LessOrEqual;
    case SkrBinary::Operator::GreaterThan: return Operator::GreaterThan;
    case SkrBinary::Operator::GreaterOrEqual: return Operator::GreaterOrEqual;
    default:
        sparkError("RvaBinary", "Failed to map skr op (%d) to rva op", op);
        return Operator::Plus;
    }
}
