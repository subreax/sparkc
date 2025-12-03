#pragma once
#include "RvaInstruction.h"

class RvaCall : public RvaInstruction {
public:
    RvaCall(StringRef funName);

    void emit(RvListing& listing) override;
    StringRef getFunName() const;

private:
    StringRef funName;
};
