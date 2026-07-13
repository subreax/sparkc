#pragma once
#include "Scope.h"
#include "sparkc/common/IdentifierGen.h"
#include "sparkc/frontend/ast/everything.h"
#include "sparkc/symbol/SymbolTable.h"

class IdentifierResolution {
public:
    IdentifierResolution(
        SymbolTable& symbolTable,
        TypeTable& typeTable,
        IdentifierGen& idGen
    );

    void resolve(AstProgram* program);

private:
    void addExistingDeclarationsToScope();

    StringRef declareVar(StringRef name, SymbolType* type);
    void declareFunction(AstFunction* func);
    void declareStruct(AstStruct* it);

    void resolve(AstProgItem* progItem);
    void resolve(AstFunction* it);
    void resolve(AstStruct* it);
    void resolve(AstBlock* block);
    void resolve(AstBlockItem* item);
    void resolve(AstDeclaration* decl);
    void resolve(AstStatement* st);
    void resolve(AstExp* exp);

    void checkDeclaration(StringRef name, ScopeItem::Kind kind);

    Scope scope;
    IdentifierGen& idGen;
    SymbolTable& symbolTable;
    TypeTable& typeTable;
};