#pragma once
#include "RvaInstruction.h"

class RvaDLoad : public RvaInstruction {
public:
    RvaDLoad(RvaRegister* dst, RvaData* src, RvaRegister* tempOffsetReg)
        : RvaInstruction(Kind::DLoad)
        , dst(dst)
        , src(src)
        , tempOffsetReg(tempOffsetReg) { }

    const RvaRegister* getDst() const { return dst; }
    const RvaData* getSrc() const { return src; }
    const RvaRegister* getTempOffsetReg() const { return tempOffsetReg; }

    void emit(RvListing& listing) override;

private:
    RvaRegister* dst;
    RvaData* src;
    RvaRegister* tempOffsetReg;
};