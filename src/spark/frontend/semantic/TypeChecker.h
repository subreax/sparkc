#pragma once
#include "../../common/symbol/SymbolTable.h"
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
        if (item->getType() == AstBlockItem::Type::Declaration) {
            auto* declItem = (AstDeclBlockItem*) item;
            typeCheck(declItem->getDeclaration());
        }
        else if (item->getType() == AstBlockItem::Type::Statement) {
            auto* stItem = (AstStatementBlockItem*) item;
            typeCheck(stItem->getStatement());
        }
    }

    void typeCheck(AstDeclaration* decl) {
        if (decl->getType() == AstDeclaration::Type::Var) {
            auto* varDecl = (AstVarDeclaration*) decl;
            auto* initExp = varDecl->getInitializer();
            if (initExp != nullptr) {
                typeCheck(initExp);
            }
        }
    }

    void typeCheck(AstStatement* st) {
        auto type = st->getType();
        if (type == AstStatement::Type::Expression) {
            auto* expSt = (AstExpressionStatement*) st;
            typeCheck(expSt->getExpression());
        }
        else if (type == AstStatement::Type::Return) {
            auto* retSt = (AstReturnStatement*) st;
            typeCheck(retSt->getExpression());
        }
    }

    void typeCheck(AstExp* exp) {
        auto type = exp->getType();
        if (type == AstExp::EXP_VAR) {
            auto* var = (AstVar*) exp;
            auto* varType = table.get(var->getIdentifier());
            if (varType->getKind() == SymbolType::Kind::Function) {
                throw TypeException("Using variable as a function: '" + std::string(var->getIdentifier()) + "'");
            }
        }
        else if (type == AstExp::EXP_FUN_CALL) {
            auto* call = (AstFunCall*) exp;
            auto* funType = (SymbolFunctionType*) table.get(call->getFunName());
            if (funType->getKind() != SymbolType::Kind::Function) {
                throw TypeException("Function '" + std::string(call->getFunName()) + "' doesn't exist");
            }

            auto funParamsCount = funType->getParams().size();
            if (call->getArgs().size() != funParamsCount) {
                throw TypeException("Function '" + std::string(call->getFunName()) + "' called with wrong number of arguments");
            }
        }
        else if (type == AstExp::EXP_BINARY) {
            auto* bin = (AstBinaryExp*) exp;
            typeCheck(bin->getLeft());
            typeCheck(bin->getRight());
        }
        else if (type == AstExp::EXP_ASSIGNMENT) {
            auto* ass = (AstAssignment*) exp;
            auto varType = ass->getVar()->getType();
            if (varType != AstExp::EXP_VAR) {
                throw TypeException("Expressions can only be assigned to variables, not " + std::to_string(varType));
            }
            typeCheck(ass->getExp());
        }
    }

    SymbolTable& table;
};