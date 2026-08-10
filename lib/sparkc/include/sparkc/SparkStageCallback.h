#pragma once
#include <vector>
#include "sparkc/symbol/SymbolTable.h"
#include "sparkc/frontend/ast/AstProgItem.h"
#include "sparkc/skr/SkrFunction.h"
#include "sparkc/common/cfg/CfGraph.h"
#include "sparkc/backend/rv/instr/RvaInstruction.h"
#include "sparkc/BuildResult.h"

class SparkStageCallback {
public:
    virtual ~SparkStageCallback() = default;

    virtual void onAstBuild(AstProgItem* item) { }
    virtual void onEmitSkrFunc(SkrFunction* skrFunc) { }
    virtual void onCfgCreated(StringRef funName, int iteration, CfGraph<SkrInstruction*>* graph) { }
    virtual void onEmitRva(const std::vector<RvaInstruction*>& rva) { }
    virtual void onReplaceRvaPseudo(const std::vector<RvaInstruction*>& rva) { }
    virtual void onFixRva(const std::vector<RvaInstruction*>& rva) { }
    virtual void onBinary(const BuildResult& buildResult) { }

    SymbolTable& getSymbolTable() {
        return *symTable;
    }

    void setSymbolTable(SymbolTable& symTable) {
        this->symTable = &symTable;
    }

private:
    SymbolTable* symTable = nullptr;
};
