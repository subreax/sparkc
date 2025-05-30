#pragma once
#include "../../symbol/SymbolTable.h"
#include "../../common/Error.h"
#include "../ast/AstProgram.h"
#include "../ast/declaration/AstVarDeclaration.h"
#include "../ast/statement/everything.h"
#include "../ast/exp/everything.h"
#include "TypeException.h"

class TypeChecker {
public:
    TypeChecker(SymbolTable& table) : table(table) {  }

    void typeCheck(AstProgram* prog) {
        for (auto* func : prog->functions) {
            for (auto* item : func->getBlockItems()) {
                typeCheck(item);
            }
        }
    }

private:
    void typeCheck(AstBlockItem* item) {
        if (item->kind == AstBlockItem::Kind::Declaration) {
            auto* declItem = (AstDeclBlockItem*) item;
            typeCheck(declItem->getDeclaration());
        }
        else if (item->kind == AstBlockItem::Kind::Statement) {
            auto* stItem = (AstStatementBlockItem*) item;
            typeCheck(stItem->getStatement());
        }
        else {
            sparkError("TypeChecker", "Unknown AstBlockItem: %d", item->kind);
        }
    }

    void typeCheck(AstDeclaration* decl) {
        if (decl->kind == AstDeclaration::Kind::Var) {
            auto* varDecl = (AstVarDeclaration*) decl;
            auto* initExp = varDecl->getInitializer();
            if (initExp != nullptr) {
                typeCheck(initExp);
            }
        }
    }

    void typeCheck(AstStatement* st) {
        auto kind = st->kind;
        if (kind == AstStatement::Kind::Expression) {
            auto* expSt = (AstExpressionStatement*) st;
            typeCheck(expSt->getExpression());
        }
        else if (kind == AstStatement::Kind::Return) {
            auto* retSt = (AstReturnStatement*) st;
            typeCheck(retSt->getExpression());
        }
        else if (kind == AstStatement::Kind::If) {
            auto* it = (AstIfStatement*) st;
            typeCheck(it->getCondition());
            typeCheck(it->getTrueBranch());
            auto* falseBranch = it->getFalseBranch();
            if (falseBranch != nullptr) {
                typeCheck(falseBranch);
            }
        }
    }



    // AstExp

    void typeCheck(AstExp* exp) {
        auto kind = exp->kind;
        switch (kind) {
        case AstExp::Kind::Constant:
            break;
            
        case AstExp::Kind::Binary:
            typeCheck((AstBinaryExp*) exp);
            break;

        case AstExp::Kind::Var:
            typeCheck((AstVar*) exp);
            break;

        case AstExp::Kind::Assignment:
            typeCheck((AstAssignment*) exp);
            break;

        case AstExp::Kind::FunCall:
            typeCheck((AstFunCall*) exp);
            break;

        default:
            sparkError("TypeChecker", "Unhandled AstExp: %s (%d)", AstExp::kindToString(kind), kind);
        }
    }

    void typeCheck(AstVar* var) {
        auto* type = table.get(var->getIdentifier());
        if (type->kind == SymbolType::Kind::Function) {
            throw TypeException("Using variable as a function: '" + std::string(var->getIdentifier()) + "'");
        }
    }
    
    void typeCheck(AstBinaryExp* bin) {
        typeCheck(bin->getLeft());
        typeCheck(bin->getRight());
    }

    void typeCheck(AstFunCall* call) {
        auto* funType = (SymbolFunctionType*) table.get(call->getFunName());
        if (funType->kind != SymbolType::Kind::Function) {
            throw TypeException("Function '" + std::string(call->getFunName()) + "' doesn't exist");
        }

        auto funParamsCount = funType->getParams().size();
        if (call->getArgs().size() != funParamsCount) {
            throw TypeException("Function '" + std::string(call->getFunName()) + "' called with wrong number of arguments");
        }
    }

    void typeCheck(AstAssignment* ass) {
        auto kind = ass->getVar()->kind;
        if (kind != AstExp::Kind::Var) {
            throw TypeException(std::string("Expressions can only be assigned to variables, not to a ") + AstExp::kindToString(kind));
        }
        typeCheck(ass->getExp());
    }

    SymbolTable& table;
};