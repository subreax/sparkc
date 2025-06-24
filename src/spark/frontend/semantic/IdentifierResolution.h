#pragma once
#include "../ast/AstProgram.h"
#include "../ast/exp/everything.h"
#include "../ast/declaration/AstVarDeclaration.h"
#include "../../symbol/SymbolTable.h"

class IdentifierResolution {
public:
    IdentifierResolution(SymbolTable& table) 
        : table(table) {  }

    void resolve(AstProgram* program) {
        for (auto* func : program->functions) {
            declareFunction(func);
        }

        for (auto* func : program->functions) {
            resolve(func->getBlock());
        }
    }

private:
    void declareFunction(AstFunction* func) {
        const auto& params = func->getParams();
        auto& typeAlloc = table.getTypeAllocator();
        BoundArray<SymbolType*> paramTypes = BoundArray<SymbolType*>::create(params.size(), typeAlloc);
        for (size_t i = 0; i < params.size(); i++) {
            AstFunParam* param = params[i];
            auto* type = param->getType();
            paramTypes[i] = type;
            table.declare(param->getIdentifier(), type);
        }

        SymbolFunctionType* funcType = typeAlloc.create<SymbolFunctionType>(func->getReturnType(), paramTypes);
        table.declare(func->getName(), funcType);
    }

    void resolve(const AstBlock* block) {
        for (AstBlockItem* item : block->getItems()) {
            resolve(item);
        }
    }

    void resolve(AstBlockItem* item) {
        auto kind = item->kind;
        if (kind == AstBlockItem::Kind::Declaration) {
            auto* declItem = (AstDeclBlockItem*) item;
            resolve(declItem->getDeclaration());
        }
        else if (kind == AstBlockItem::Kind::Statement) {
            auto* stItem = (AstStatementBlockItem*) item;
            resolve(stItem->getStatement());
        }
    }

    void resolve(AstDeclaration* decl) {
        if (decl->kind == AstDeclaration::Kind::Var) {
            auto* varDecl = (AstVarDeclaration*) decl;
            table.declare(varDecl->getName(), varDecl->getType());
        }
    }

    void resolve(AstStatement* st) {
        if (st->kind == AstStatement::Kind::If) {
            auto* it = (AstIfStatement*) st;
            resolve(it->getTrueBranch());
            auto* falseBranch = it->getFalseBranch();
            if (falseBranch != nullptr) {
                resolve(falseBranch);
            }
        }
        else if (st->kind == AstStatement::Kind::While) {
            auto* it = (AstWhileStatement*) st;
            resolve(it->getStatement());
        }
        else if (st->kind == AstStatement::Kind::Compound) {
            auto* it = (AstCompoundStatement*) st;
            resolve(it->getBlock());
        }
    }

    SymbolTable& table;
};