#pragma once
#include "statement/AstStatement.h"
#include "declaration/AstDeclaration.h"

class AstBlockItem {
public:
    enum class Kind { Declaration, Statement };

    AstBlockItem(Kind kind) : kind(kind) {  }

    const Kind kind;
};


class AstDeclBlockItem : public AstBlockItem {
public:
    AstDeclBlockItem(AstDeclaration* decl) : AstBlockItem(Kind::Declaration), decl(decl) {  }

    AstDeclaration* getDeclaration() { return decl; }
    const AstDeclaration* getDeclaration() const { return decl; }
    
private:
    AstDeclaration* decl;
};


class AstStatementBlockItem : public AstBlockItem {
public:
    AstStatementBlockItem(AstStatement* statement) : AstBlockItem(Kind::Statement), statement(statement) { }

    AstStatement* getStatement() { return statement; }
    const AstStatement* getStatement() const { return statement; }

private:
    AstStatement* statement;
};