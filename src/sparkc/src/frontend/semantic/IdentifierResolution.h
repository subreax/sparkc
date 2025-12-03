#pragma once
#include "sparkc/frontend/ast/everything.h"
#include "sparkc/symbol/SymbolTable.h"
#include "sparkc/common/IdentifierGen.h"
#include "Scope.h"

class IdentifierResolution {
public:
    IdentifierResolution(SymbolTable& symbolTable, TypeTable& typeTable, IdentifierGen& idGen, size_t scopeMem);

    void resolve(AstProgram* program);

private:
    void declareFunction(AstFunction* func);
    void declareStruct(AstStruct* it);
    StringRef declareVar(StringRef name, SymbolType* type);

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
};