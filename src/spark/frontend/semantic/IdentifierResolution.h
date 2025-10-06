#pragma once
#include "../ast/everything.h"
#include "../../symbol/SymbolTable.h"
#include "../../common/IdentifierGen.h"
#include "Scope.h"

class IdentifierResolution {
public:
    IdentifierResolution(SymbolTable& symbolTable, TypeTable& typeTable, IdentifierGen& idGen, size_t scopeMem)
        : scope(symbolTable, typeTable, scopeMem)
        , idGen(idGen) {  }

    void resolve(AstProgram* program) {
        for (auto* it : program->items) {
            if (it->kind == AstProgItem::Kind::Function) {
                declareFunction((AstFunction*) it);
            }
        }

        for (auto* it : program->items) {
            resolve(it);
        }
    }

private:
    void declareFunction(AstFunction* func) {
        const auto& params = func->getParams();
        std::vector<SymbolType*> paramTypes(params.size());
        for (size_t i = 0; i < params.size(); i++) {
            paramTypes[i] = params[i]->getType();
        }
        scope.declareFunc(func->getName(), func->getReturnType(), paramTypes);
    }

    void declareStruct(AstStruct* it) {
        std::vector<StructField> fields;
        for (auto* field : it->getFields()) {
            fields.emplace_back(field->getName(), field->getType());
        }

        scope.declareStruct(it->getTag(), fields);
    }

    void resolve(AstProgItem* progItem) {
        if (progItem->kind == AstProgItem::Kind::Struct) {
            resolve((AstStruct*) progItem);
        }
        else if (progItem->kind == AstProgItem::Kind::Function) {
            resolve((AstFunction*) progItem);
        }
        else {
            sparkError("IdentifierResolution", "Unknown AstProgItem: %d", progItem->kind);
        }
    }

    void resolve(AstFunction* it) {
        scope.open();

        for (auto* param : it->getParams()) {
            param->setId(declareVar(param->getId(), param->getType()));
        }

        resolve(it->getBlock());

        scope.close();
    }

    void resolve(AstStruct* it) {
        declareStruct(it);
    }

    void resolve(AstBlock* block) {
        scope.open();
        for (AstBlockItem* item : block->getItems()) {
            resolve(item);
        }
        scope.close();
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
        else {
            sparkError("IdentifierResolution", "Unknown AstBlockItem: %d", kind);
        }
    }

    void resolve(AstDeclaration* decl) {
        if (decl->kind == AstDeclaration::Kind::Var) {
            auto* varDecl = (AstVarDeclaration*) decl;
            if (varDecl->getInitializer() != nullptr) {
                resolve(varDecl->getInitializer());
            }
            varDecl->setId(declareVar(varDecl->getId(), varDecl->getType()));
        }
        else {
            sparkError("IdentifierResolution", "Unknown AstDeclaration: %d", decl->kind);
        }
    }

    void resolve(AstStatement* st) {
        if (st->kind == AstStatement::Kind::Return) {
            resolve(((AstReturnStatement*) st)->getExpression());
        }
        else if (st->kind == AstStatement::Kind::Expression) {
            resolve(((AstExpressionStatement*) st)->getExpression());
        }
        else if (st->kind == AstStatement::Kind::If) {
            auto* it = (AstIfStatement*) st;
            resolve(it->getCondition());
            resolve(it->getTrueBranch());
            auto* falseBranch = it->getFalseBranch();
            if (falseBranch != nullptr) {
                resolve(falseBranch);
            }
        }
        else if (st->kind == AstStatement::Kind::While) {
            auto* it = (AstWhileStatement*) st;
            resolve(it->getCondition());
            resolve(it->getStatement());
        }
        else if (st->kind == AstStatement::Kind::Compound) {
            auto* it = (AstCompoundStatement*) st;
            resolve(it->getBlock());
        }
        else {
            sparkError("IdentifierResolution", "Unknown AstStatement: %d", st->kind);
        }
    }

    void resolve(AstExp* exp) {
        auto kind = exp->kind;
        if (kind == AstExp::Kind::Constant) {
        }
        else if (kind == AstExp::Kind::Binary) {
            auto* it = (AstBinaryExp*) exp;
            resolve(it->getLeft());
            resolve(it->getRight());
        }
        else if (kind == AstExp::Kind::Var) {
            auto* it = (AstVar*) exp;
            it->setId(scope.get(it->getId(), ScopeItem::Kind::Var).id);
        }
        else if (kind == AstExp::Kind::Assignment) {
            auto* it = (AstAssignment*) exp;
            resolve(it->getVar());
            resolve(it->getExp());
        }
        else if (kind == AstExp::Kind::FunCall) {
            auto* it = (AstFunCall*) exp;
            checkDeclaration(it->getFunName(), ScopeItem::Kind::Func);
            for (auto* arg : it->getArgs()) {
                resolve(arg);
            }
        }
        else if (kind == AstExp::Kind::Cast) {
            auto* it = (AstCast*) exp;
            resolve(it->getExp());
        }
        else if (kind == AstExp::Kind::Dereference) {
            auto* it = (AstDereference*) exp;
            resolve(it->getExpression());
        }
        else if (kind == AstExp::Kind::AddrOf) {
            auto* it = (AstAddrOf*) exp;
            resolve(it->getExpression());
        }
        else if (kind == AstExp::Kind::Dot) {
            auto* it = (AstDot*) exp;
            resolve(it->getFrom());
        }
        else if (kind == AstExp::Kind::StructInit) {
            auto* it = (AstStructInit*) exp;
            checkDeclaration(it->getTag(), ScopeItem::Kind::Struct);
            for (auto* arg : it->getArgs()) {
                resolve(arg);
            }
        }
        else {
            sparkError("IdentifierResolution", "Unknown AstExp: %d", kind);
        }
    }

    StringRef declareVar(StringRef name, SymbolType* type) {
        auto id = idGen.unique(name);
        scope.declareVar(name, id, type);
        return id;
    }

    void checkDeclaration(StringRef name, ScopeItem::Kind kind) {
        scope.get(name, kind);
    }

    Scope scope;
    IdentifierGen& idGen;
};