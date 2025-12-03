#pragma once
#include "sparkc/frontend/ast/everything.h"
#include "sparkc/common/Error.h"
#include "sparkc/symbol/SymbolTable.h"
#include "sparkc/type/TypeTable.h"
#include "TypeException.h"

class TypeChecker {
  public:
    TypeChecker(
        SymbolTable& symbolTable,
        TypeTable& typeTable,
        Allocator& astAllocator);

    void typeCheck(AstProgram* prog);

  private:
    void typeCheck(const AstBlock* block, SymbolType* retType);
    void typeCheck(AstBlockItem* item, SymbolType* retType);
    void typeCheck(AstDeclaration* decl);
    void typeCheck(AstStatement* st, SymbolType* retType);

    void typeCheck(AstExp* exp);
    void typeCheck(AstVar* var);
    void typeCheck(AstBinaryExp* bin);
    void typeCheck(AstFunCall* call);
    void typeCheck(AstAssignment* ass);
    void typeCheck(AstDot* it);
    void typeCheck(AstStructInit* it);

    static SymbolType* getCommonType(AstExp* e1, AstExp* e2);
    static SymbolType* getCommonType(SymbolType* t1, SymbolType* t2);
    static SymbolType* dereference(SymbolType* t);

    AstExp* dereference(AstExp* exp);

    AstExp* cast(AstExp* exp, SymbolType* targetType);

    SymbolTable& symbolTable;
    TypeTable& typeTable;
    Allocator& allocator;
};