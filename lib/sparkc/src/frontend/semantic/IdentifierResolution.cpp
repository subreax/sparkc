#include "IdentifierResolution.h"

IdentifierResolution::IdentifierResolution(
    SymbolTable& symbolTable,
    TypeTable& typeTable,
    IdentifierGen& idGen
)
    : idGen(idGen)
    , symbolTable(symbolTable)
    , typeTable(typeTable) {
    addExistingDeclarationsToScope();
}

void IdentifierResolution::resolve(AstProgram* program) {
    for (auto* it : program->items) {
        if (it->kind == AstProgItem::Kind::Function) {
            declareFunction((AstFunction*) it);
        }
    }

    for (auto* it : program->items) {
        resolve(it);
    }
}

void IdentifierResolution::addExistingDeclarationsToScope() {
    for (const auto& structEntry : typeTable) {
        scope.declareStruct(structEntry.first);
    }

    for (const auto& entry : symbolTable) {
        scope.declareFunc(entry.first);
    }
}

StringRef IdentifierResolution::declareVar(StringRef name, SymbolType* type) {
    auto id = idGen.unique(name);
    scope.declareVar(name, id);
    symbolTable.declareVar(id, type);
    return id;
}

void IdentifierResolution::declareFunction(AstFunction* func) {
    const auto& params = func->getParams();
    std::vector<SymbolType*> paramTypes(params.size());
    for (size_t i = 0; i < params.size(); i++) {
        paramTypes[i] = params[i]->getType();
    }

    scope.declareFunc(func->getName());
    symbolTable.declareFunc(func->getName(), func->getReturnType(), paramTypes);
}

void IdentifierResolution::declareStruct(AstStruct* it) {
    std::vector<StructField> fields;
    for (auto* field : it->getFields()) {
        fields.emplace_back(field->getName(), field->getType());
    }

    scope.declareStruct(it->getTag());
    typeTable.declare(it->getTag(), fields);
}

void IdentifierResolution::resolve(AstProgItem* progItem) {
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

void IdentifierResolution::resolve(AstFunction* it) {
    scope.open();

    for (auto* param : it->getParams()) {
        param->setId(declareVar(param->getId(), param->getType()));
    }

    resolve(it->getBlock());

    scope.close();
}

void IdentifierResolution::resolve(AstStruct* it) { declareStruct(it); }

void IdentifierResolution::resolve(AstBlock* block) {
    scope.open();
    for (AstBlockItem* item : block->getItems()) {
        resolve(item);
    }
    scope.close();
}

void IdentifierResolution::resolve(AstBlockItem* item) {
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

void IdentifierResolution::resolve(AstDeclaration* decl) {
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

void IdentifierResolution::resolve(AstStatement* st) {
    switch (st->kind) {
    case AstStatement::Kind::Return: {
        auto* it = (AstReturnStatement*) st;
        resolve(it->getExpression());
        break;
    }

    case AstStatement::Kind::Expression: {
        auto* it = (AstExpressionStatement*) st;
        resolve(it->getExpression());
        break;
    }

    case AstStatement::Kind::If: {
        auto* it = (AstIfStatement*) st;
        resolve(it->getCondition());
        resolve(it->getTrueBranch());

        if (auto* falseBranch = it->getFalseBranch()) {
            resolve(falseBranch);
        }
        break;
    }

    case AstStatement::Kind::While: {
        auto* it = (AstWhileStatement*) st;
        resolve(it->getCondition());
        resolve(it->getStatement());
        break;
    }

    case AstStatement::Kind::Compound: {
        auto* it = (AstCompoundStatement*) st;
        resolve(it->getBlock());
        break;
    }

    default: {
        sparkError("IdentifierResolution", "Unknown AstStatement: %d", st->kind);
        break;
    }
    }
}

void IdentifierResolution::resolve(AstExp* exp) {
    switch (exp->kind) {
    case AstExp::Kind::Constant:
        break;

    case AstExp::Kind::Binary: {
        auto* it = (AstBinaryExp*) exp;
        resolve(it->getLeft());
        resolve(it->getRight());
        break;
    }

    case AstExp::Kind::Var: {
        auto* it = (AstVar*) exp;
        it->setId(scope.get(it->getId(), ScopeItem::Kind::Var).id);
        break;
    }

    case AstExp::Kind::Assignment: {
        auto* it = (AstAssignment*) exp;
        resolve(it->getVar());
        resolve(it->getExp());
        break;
    }

    case AstExp::Kind::FunCall: {
        auto* it = (AstFunCall*) exp;
        checkDeclaration(it->getFunName(), ScopeItem::Kind::Func);
        for (auto* arg : it->getArgs()) {
            resolve(arg);
        }
        break;
    }

    case AstExp::Kind::Cast: {
        auto* it = (AstCast*) exp;
        resolve(it->getExp());
        break;
    }

    case AstExp::Kind::Dereference: {
        auto* it = (AstDereference*) exp;
        resolve(it->getExpression());
        break;
    }

    case AstExp::Kind::AddrOf: {
        auto* it = (AstAddrOf*) exp;
        resolve(it->getExpression());
        break;
    }

    case AstExp::Kind::Dot: {
        auto* it = (AstDot*) exp;
        resolve(it->getFrom());
        break;
    }

    case AstExp::Kind::StructInit: {
        auto* it = (AstStructInit*) exp;
        checkDeclaration(it->getTag(), ScopeItem::Kind::Struct);
        for (auto* arg : it->getArgs()) {
            resolve(arg);
        }
        break;
    }

    default:
        sparkError("IdentifierResolution", "Unknown AstExp: %d", exp->kind);
        break;
    }
}

void IdentifierResolution::checkDeclaration(StringRef name, ScopeItem::Kind kind) {
    scope.get(name, kind);
}
