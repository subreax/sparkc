#pragma once
#include "RvaInstruction.h"

class RvaDStore : public RvaInstruction {
public:
    RvaDStore(RvaData* dst, RvaRegister* src, RvaRegister* tempOffsetReg)
        : RvaInstruction(Kind::DStore)
        , dst(dst)
        , src(src)
        , tempOffsetReg(tempOffsetReg) { }

    const RvaData* getDst() const { return dst; }
    const RvaRegister* getSrc() const { return src; }
    const RvaRegister* getTempOffsetReg() const { return tempOffsetReg; }

    void emit(RvListing& listing) override;

private:
    RvaData* dst;
    RvaRegister* src;
    RvaRegister* tempOffsetReg;
};