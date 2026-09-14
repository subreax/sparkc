#pragma once
#include <vector>
#include "sparkc/symbol/SymbolTable.h"
#include "sparkc/frontend/ast/AstProgItem.h"
#include "sparkc/skr/SkrFunction.h"
#include "sparkc/skr/optimizer/SkrCfg.h"
#include "sparkc/backend/rv/instr/RvaInstruction.h"
#include "sparkc/BuildResult.h"

class SparkStageCallback {
public:
    virtual ~SparkStageCallback() = default;

    virtual void onAstBuild(AstProgItem*) { }
    virtual void onEmitSkr(SkrProgItem*) { }
    virtual void onCfgCreated(StringRef, int, SkrCfg&) { }
    virtual void onEmitRva(const std::vector<RvaInstruction*>&) { }
    virtual void onReplaceRvaPseudo(const std::vector<RvaInstruction*>&) { }
    virtual void onFixRva(const std::vector<RvaInstruction*>&) { }
    virtual void onBinary(const BuildResult&) { }

    SymbolTable& getSymbolTable() {
        return *symTable;
    }

    void setSymbolTable(SymbolTable& symTable) {
        this->symTable = &symTable;
    }

private:
    SymbolTable* symTable = nullptr;
};
