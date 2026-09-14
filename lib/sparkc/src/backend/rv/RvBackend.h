#pragma once
#include <memory>
#include "sparkc/common/alloc/Allocator.h"
#include "sparkc/SparkStageCallback.h"
#include "sparkc/symbol/SymbolTable.h"
#include "sparkc/size/SymbolSize.h"
#include "sparkc/common/IdentifierGen.h"
#include "Skr2RvaPseudo.h"
#include "RvaPseudoReplacer.h"
#include "RvaFixer.h"

class RvBackend {
public:
    RvBackend(Allocator& pool1, Allocator& pool2, SymbolTable& symTable, SymbolSize& symSize, IdentifierGen& idGen, SparkStageCallback& stageCallback)
        : pool1(pool1)
        , pool2(pool2)
        , symTable(symTable)
        , symSize(symSize)
        , idGen(idGen)
        , stageCallback(stageCallback) { }

    void emit(SkrProgItem* skrItem) {
        tempRvas.clear();
        rvas.clear();
        pool1.reset();
        stackFrame = std::make_shared<StackFrame>(pool1);

        Skr2RvaPseudo::emit(skrItem, pool1, idGen, symTable, symSize, *stackFrame, tempRvas);
        stageCallback.onEmitRva(tempRvas);
    }

    void replacePseudo() {
        RvaPseudoReplacer::replace(tempRvas, *stackFrame, symSize);
        stageCallback.onReplaceRvaPseudo(tempRvas);
    }

    const std::vector<RvaInstruction*>& fix() {
        pool2.reset();
        RvaFixer::fix(tempRvas, rvas, pool2);
        stageCallback.onFixRva(rvas);
        return rvas;
    }

private:
    // todo: replace them with something like RvaFactory
    Allocator& pool1;
    Allocator& pool2;
    SymbolTable& symTable;
    SymbolSize& symSize;
    IdentifierGen& idGen;

    std::shared_ptr<StackFrame> stackFrame;
    SparkStageCallback& stageCallback;

    std::vector<RvaInstruction*> tempRvas;
    std::vector<RvaInstruction*> rvas;
};
