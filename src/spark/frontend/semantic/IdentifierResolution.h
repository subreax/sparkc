#pragma once
#include "../ast/AstProgram.h"
#include "../ast/exp/everything.h"
#include "../ast/declaration/AstVarDeclaration.h"
#include "../../common/symbol/SymbolTable.h"

class IdentifierResolution {
public:
    IdentifierResolution(SymbolTable& table, LinearAllocator& typeAlloc) 
        : table(table)
        , typeAlloc(typeAlloc) {  }

    void resolve(AstProgram* program) {
        for (auto* func : program->functions) {
            declareFunction(func);
        }

        for (auto* func : program->functions) {
            for (AstBlockItem* item : func->getBlockItems()) {
                resolve(item);
            }
        }
    }

private:
    void declareFunction(AstFunction* func) {
        const auto& params = func->getParams();
        BoundArray<SymbolType*> paramTypes = BoundArray<SymbolType*>::create(params.size(), typeAlloc);
        for (size_t i = 0; i < params.size(); i++) {
            paramTypes[i] = SymbolIntType::getInstance();
            table.declare(params[i]->getIdentifier(), SymbolIntType::getInstance());
        }

        SymbolType* retType = SymbolIntType::getInstance();
        SymbolFunctionType* funcType = typeAlloc.create<SymbolFunctionType>(retType, paramTypes);
        table.declare(func->getName(), funcType);
    }

    void resolve(AstBlockItem* item) {
        auto type = item->getType();
        if (type == AstBlockItem::Type::Declaration) {
            resolve((AstDeclaration*) item);
        }
    }

    void resolve(AstDeclaration* decl) {
        auto type = decl->getType();
        if (type == AstDeclaration::Type::Var) {
            auto* varDecl = (AstVarDeclaration*) decl;
            table.declare(varDecl->getName(), SymbolIntType::getInstance());
        }
    }

    SymbolTable& table;
    LinearAllocator& typeAlloc;
};