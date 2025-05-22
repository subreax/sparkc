#pragma once
#include "statement/AstStatement.h"
#include "declaration/AstDeclaration.h"

class AstBlockItem {
public:
    enum class Type { Declaration, Statement };

    AstBlockItem(Type type) : type(type) { }
    Type getType() const { return type; }

private:
    const Type type;
};


class AstDeclBlockItem : public AstBlockItem {
public:
    AstDeclBlockItem(AstDeclaration* decl) : AstBlockItem(Type::Declaration), decl(decl) {  }

    AstDeclaration* getDeclaration() { return decl; }
    
private:
    AstDeclaration* decl;
};


class AstStatementBlockItem : public AstBlockItem {
public:
    AstStatementBlockItem(AstStatement* statement) : AstBlockItem(Type::Statement), statement(statement) { }

    AstStatement* getStatement() { return statement; }

private:
    AstStatement* statement;
};