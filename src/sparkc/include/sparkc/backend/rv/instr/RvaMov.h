#pragma once
#include "RvaInstruction.h"

class RvaMov : public RvaInstruction {
public:
    RvaMov(RvaValue* to, RvaValue* from);

    void emit(RvListing& listing) override;

    RvaValue* to;
    RvaValue* from;

private:
    static bool isImm11(int32_t imm);
    static int32_t lo(int32_t imm);
    static int32_t hi(int32_t imm);
    static int32_t sext11(int32_t imm11);
};
