#pragma once
#include "../../symbol/SymbolTable.h"
#include "../../type/TypeTable.h"
#include "../../common/Error.h"
#include "../ast/AstProgram.h"
#include "../ast/declaration/AstVarDeclaration.h"
#include "../ast/statement/everything.h"
#include "../ast/exp/everything.h"
#include "TypeException.h"

class TypeChecker {
public:
    TypeChecker(SymbolTable& symbolTable, TypeTable& typeTable, Allocator& astAllocator) 
        : symbolTable(symbolTable) 
        , typeTable(typeTable)
        , allocator(astAllocator) {  }

    void typeCheck(AstProgram* prog) {
        for (auto* item : prog->items) {
            if (item->kind == AstProgItem::Kind::Function) {
                auto* func = (AstFunction*) item;
                typeCheck(func->getBlock(), func->getReturnType());
            }
            else if (item->kind == AstProgItem::Kind::Struct) {
            }
            else {
                sparkError("TypeChecker", "Unknown AstProgItem: %d", item->kind);
            }
        }
    }

private:
    void typeCheck(const AstBlock* block, SymbolType* retType) {
        for (auto* item : block->getItems()) {
            typeCheck(item, retType);
        }
    }

    void typeCheck(AstBlockItem* item, SymbolType* retType) {
        if (item->kind == AstBlockItem::Kind::Declaration) {
            auto* declItem = (AstDeclBlockItem*) item;
            typeCheck(declItem->getDeclaration());
        }
        else if (item->kind == AstBlockItem::Kind::Statement) {
            auto* stItem = (AstStatementBlockItem*) item;
            typeCheck(stItem->getStatement(), retType);
        }
        else {
            sparkError("TypeChecker", "Unknown AstBlockItem: %d", item->kind);
        }
    }

    void typeCheck(AstDeclaration* decl) {
        if (decl->kind == AstDeclaration::Kind::Var) {
            auto* varDecl = (AstVarDeclaration*) decl;
            auto* initExp = varDecl->getInitializer();
            if (varDecl->getType()->kind == SymbolType::Kind::Pointer && initExp == nullptr) {
                throw TypeException("Reference is not initialized");
            }

            if (initExp != nullptr) {
                typeCheck(initExp);
                varDecl->setInitializer(cast(initExp, varDecl->getType()));
            }
        }
    }

    void typeCheck(AstStatement* st, SymbolType* retType) {
        auto kind = st->kind;
        if (kind == AstStatement::Kind::Expression) {
            auto* expSt = (AstExpressionStatement*) st;
            typeCheck(expSt->getExpression());
        }
        else if (kind == AstStatement::Kind::Return) {
            auto* retSt = (AstReturnStatement*) st;
            auto* exp = retSt->getExpression();
            typeCheck(exp);
            retSt->setExpression(cast(exp, retType));
        }
        else if (kind == AstStatement::Kind::If) {
            auto* it = (AstIfStatement*) st;
            typeCheck(it->getCondition());
            typeCheck(it->getTrueBranch(), retType);
            auto* falseBranch = it->getFalseBranch();
            if (falseBranch != nullptr) {
                typeCheck(falseBranch, retType);
            }
        }
        else if (kind == AstStatement::Kind::While) {
            auto* it = (AstWhileStatement*) st;
            typeCheck(it->getCondition());
            typeCheck(it->getStatement(), retType);
        }
        else if (kind == AstStatement::Kind::Compound) {
            typeCheck(((AstCompoundStatement*) st)->getBlock(), retType);
        }
    }



    // AstExp

    void typeCheck(AstExp* exp) {
        auto kind = exp->kind;
        switch (kind) {
        case AstExp::Kind::Constant: break;
        case AstExp::Kind::Binary: typeCheck((AstBinaryExp*) exp); break;
        case AstExp::Kind::Var: typeCheck((AstVar*) exp); break;
        case AstExp::Kind::Assignment: typeCheck((AstAssignment*) exp); break;
        case AstExp::Kind::FunCall: typeCheck((AstFunCall*) exp); break;
        case AstExp::Kind::Dot: typeCheck((AstDot*) exp); break;
        default:
            sparkError("TypeChecker", "Unhandled AstExp: %s (%d)", AstExp::kindToString(kind), kind);
        }
    }

    void typeCheck(AstVar* var) {
        auto* type = symbolTable.get(var->getId());
        if (type->kind == SymbolType::Kind::Function) {
            throw TypeException("Using variable as a function: '" + var->getId().toString() + "'");
        }

        var->type = type;
    }
    
    void typeCheck(AstBinaryExp* bin) {
        typeCheck(bin->getLeft());
        typeCheck(bin->getRight());
        auto type = getCommonType(bin->getLeft(), bin->getRight());

        bin->type = type;
        bin->setLeft(cast(bin->getLeft(), type));
        bin->setRight(cast(bin->getRight(), type));
    }

    void typeCheck(AstFunCall* call) {
        auto* funType = (SymbolFunctionType*) symbolTable.get(call->getFunName());
        if (funType->kind != SymbolType::Kind::Function) {
            throw TypeException("Function '" + call->getFunName().toString() + "' doesn't exist");
        }

        auto params = funType->getParams();
        auto paramsCount = params.size();
        auto args = call->getArgs();
        if (args.size() != paramsCount) {
            throw TypeException("Function '" + call->getFunName().toString() + "' called with wrong number of arguments");
        }

        for (size_t i = 0; i < args.size(); i++) {
            auto* arg = args[i];
            auto* paramType = params[i];
            typeCheck(arg);
            args[i] = cast(arg, paramType);
        }

        call->type = funType->getReturnType();
    }

    void typeCheck(AstAssignment* ass) {
        auto kind = ass->getVar()->kind;
        if (kind != AstExp::Kind::Var && kind != AstExp::Kind::Dot) {
            throw TypeException(std::string("Expressions can only be assigned to variables or dots, not to a ") + AstExp::kindToString(kind));
        }

        typeCheck(ass->getVar());
        ass->setVar(dereference(ass->getVar()));
        ass->type = ass->getVar()->type;

        typeCheck(ass->getExp());
        ass->setExp(cast(ass->getExp(), ass->type));
    }

    void typeCheck(AstDot* it) {
        typeCheck(it->getFrom());
        it->setFrom(dereference(it->getFrom()));
        auto* fromType = it->getFrom()->type;
        if (fromType->kind != SymbolType::Kind::Structure) {
            throw TypeException("Trying to access a struct member on a non-struct type");
        }

        if (it->getField()->kind != AstExp::Kind::Var) {
            throw TypeException("Wtf is hapenned around a struct var");
        }

        auto accessedField = ((AstVar*) it->getField())->getId();

        auto* structType = (SymbolStructureType*) fromType;
        const auto& structFields = typeTable.get(structType->getTag());
        for (auto field : structFields) {
            if (field.name == accessedField) {
                it->getField()->type = field.type;
                it->type = field.type;
                return;
            }
        }

        throw TypeException("Trying to access unknown struct field");
    }

    static SymbolType* getCommonType(AstExp* e1, AstExp* e2) {
        return getCommonType(e1->type, e2->type);
    }

    static SymbolType* getCommonType(SymbolType* t1, SymbolType* t2) {
        auto k1 = t1->kind;
        auto k2 = t2->kind;
        if (k1 == k2) {
            return t1;
        }

        if (k1 == SymbolType::Kind::Function || k2 == SymbolType::Kind::Function) {
            throw TypeException("Common type with function doesn't exist");
        }
        else if (k1 == SymbolType::Kind::Integer && k2 == SymbolType::Kind::Float) {
            return t2;
        }
        else if (k1 == SymbolType::Kind::Float && k2 == SymbolType::Kind::Integer) {
            return t1;
        }

        auto* t1deref = dereference(t1);
        auto* t2deref = dereference(t2);
        if (t1deref->kind != t2deref->kind) {
            throw TypeException("References should have the same base type");
        }

        return t1deref;
    }

    static SymbolType* dereference(SymbolType* t) {
        if (t->kind == SymbolType::Kind::Pointer) {
            return ((SymbolPointerType*) t)->getVarType();
        }
        return t;
    }

    AstExp* dereference(AstExp* exp) {
        auto* type = exp->type;
        if (type->kind == SymbolType::Kind::Pointer) {
            return allocator.create<AstDereference>(exp, dereference(type));
        }
        return exp;
    }

    AstExp* cast(AstExp* exp, SymbolType* targetType) {
        if (exp->hasType(targetType)) {
            if (!arePointersCompatible(exp->type, targetType)) {
                throw TypeException("Can't cast pointer to another type");
            }
            return exp;
        }
        else if (exp->hasType(SymbolType::Kind::Pointer)) {
            return cast(dereference(exp), targetType);
        }
        else if (targetType->kind == SymbolType::Kind::Pointer) {
            if (exp->kind != AstExp::Kind::Var) {
                throw TypeException("Can't get address: exp is not AstVar");
            }

            if (exp->type->kind == dereference(targetType)->kind) {
                return allocator.create<AstAddrOf>(exp, targetType);
            } else {
                throw TypeException("Can't reference to a variable with another type");
            }
        }
        return allocator.create<AstCast>(exp, targetType);
    }

    bool arePointersCompatible(SymbolType* t1, SymbolType* t2) {
        if (t1->kind == SymbolType::Kind::Pointer && t2->kind == SymbolType::Kind::Pointer) {
            return dereference(t1)->kind == dereference(t2)->kind;
        }
        return true;
    }

    SymbolTable& symbolTable;
    TypeTable& typeTable;
    Allocator& allocator;
};