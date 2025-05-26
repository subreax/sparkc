#pragma once
#include <vector>
#include "instr/everything.h"
#include "StackFrame.h"
#include "../../common/Error.h"

class RvaPseudoReplacer {
public:
    static void replace(StackFrame& frame, std::vector<RvaInstruction*>& rvas) {
        RvaPseudoReplacer(frame).replace(rvas);
    }

    RvaPseudoReplacer(StackFrame& frame) : frame(frame) {  }

    void replace(std::vector<RvaInstruction*>& rvas) {
        for (auto* it : rvas) 
            replace(it);
        
        if (rvas.size() > 1) {
            auto frameSize = frame.getSizeAligned16();
            auto* prologue = (RvaPrologue*) rvas[1];
            if (prologue->getType() == RvaInstruction::Type::Prologue) {
                prologue->setFrameSize(frameSize);
            } else {
                sparkError("RvaPseudoReplacer", "Can't find prologue");
            }

            auto* epilogue = (RvaEpilogue*) rvas[rvas.size() - 2];
            if (epilogue->getType() == RvaInstruction::Type::Epilogue) {
                epilogue->setFrameSize(frameSize);
            } else {
                sparkError("RvaPseudoReplacer", "Can't find epilogue");
            }
        }
    }

private:
    void replace(RvaInstruction* it) {
        auto type = it->getType();
        switch (type) {
        case RvaInstruction::Type::Binary:      replace((RvaBinary*) it); break;
        case RvaInstruction::Type::Move:        replace((RvaMov*) it); break;
        case RvaInstruction::Type::Branch:      replace((RvaBranch*) it); break;
        }
    }

    void replace(RvaBinary* it) {
        replace(&it->dst);
        replace(&it->left);
        replace(&it->right);
    }

    void replace(RvaMov* it) {
        replace(&it->from);
        replace(&it->to);
    }

    void replace(RvaBranch* it) {
        replace(&it->left);
        replace(&it->right);
    }

    inline void replace(RvaValue** v) {
        if ((*v)->getType() == RvaValue::Type::PseudoReg) {
            auto* pseudo = (RvaPseudoReg*) *v;
            *v = frame.getOrPush(pseudo->getId());
        }
    }

    StackFrame& frame;
};
