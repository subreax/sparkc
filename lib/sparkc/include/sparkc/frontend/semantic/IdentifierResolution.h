#pragma once
#include "Scope.h"
#include "sparkc/frontend/ast/AstFactory.h"
#include "sparkc/common/IdentifierGen.h"
#include "sparkc/symbol/SymbolTable.h"

class IdentifierResolution {
public:
    IdentifierResolution(
        AstFactory& astFactory,
        SymbolTable& symbolTable,
        TypeTable& typeTable,
        IdentifierGen& idGen
    );

    void resolve(AstProgram* program);
    void resolve(AstProgItem* progItem);

private:
    void addExistingDeclarationsToScope();

    StringRef declareVar(StringRef name, SymbolType* type);
    void declareFunction(AstFunction* func);
    void declareStruct(AstStruct* it);

    void resolve(AstFunction* it);
    void resolve(AstStruct* it);
    void resolve(AstBlock* block);
    void resolve(AstBlockItem* item);
    void resolve(AstDeclaration* decl);
    void resolve(AstStatement* st);
    AstExp* resolveExp(AstExp* exp);
    void resolve(BoundArray<AstExp*>& array);

    void checkDeclaration(StringRef name, ScopeItem::Kind kind);

    AstStructInit* funCallToStructInit(AstFunCall* call);

    Scope scope;
    AstFactory& astf;
    IdentifierGen& idGen;
    SymbolTable& symbolTable;
    TypeTable& typeTable;
};