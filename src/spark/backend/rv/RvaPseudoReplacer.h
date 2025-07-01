#pragma once
#include <vector>
#include "instr/everything.h"
#include "StackFrame.h"
#include "../../common/Error.h"

class RvaPseudoReplacer {
public:
    static void replace(std::vector<RvaInstruction*>& rvas, StackFrame& frame) {
        RvaPseudoReplacer(frame).replace(rvas);
    }

private:
    RvaPseudoReplacer(StackFrame& frame) : frame(frame) {  }

    void replace(std::vector<RvaInstruction*>& rvas) {
        for (auto* it : rvas) 
            replace(it);
        
        if (rvas.size() > 1) {
            auto frameSize = frame.getSizeAligned16();
            auto* prologue = (RvaPrologue*) rvas[1];
            if (prologue->kind == RvaInstruction::Kind::Prologue) {
                prologue->setFrameSize(frameSize);
            } else {
                sparkError("RvaPseudoReplacer", "Can't find prologue");
            }

            auto* epilogue = (RvaEpilogue*) rvas[rvas.size() - 2];
            if (epilogue->kind == RvaInstruction::Kind::Epilogue) {
                epilogue->setFrameSize(frameSize);
            } else {
                sparkError("RvaPseudoReplacer", "Can't find epilogue");
            }
        }
    }

    void replace(RvaInstruction* it) {
        switch (it->kind) {
        case RvaInstruction::Kind::Binary:      replace((RvaBinary*) it); break;
        case RvaInstruction::Kind::Move:        replace((RvaMov*) it); break;
        case RvaInstruction::Kind::Branch:      replace((RvaBranch*) it); break;
        case RvaInstruction::Kind::Load:        replace((RvaLoad*) it); break;
        case RvaInstruction::Kind::Store:       replace((RvaStore*) it); break;
        case RvaInstruction::Kind::GetAddress:  replace((RvaGetAddress*) it); break;
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

    void replace(RvaLoad* it) {
        replace(&it->to);
        replace(&it->fromAddr);
    }
    
    void replace(RvaStore* it) {
        replace(&it->toAddr);
        replace(&it->from);
    }

    void replace(RvaGetAddress* it) {
        replace(&it->to);
        replace(&it->of);
    }

    inline void replace(RvaValue** v) {
        if ((*v)->kind == RvaValue::Kind::PseudoReg) {
            auto* pseudo = (RvaPseudoReg*) *v;
            *v = frame.getOrPush(pseudo->getId());
        }
    }

    StackFrame& frame;
};
