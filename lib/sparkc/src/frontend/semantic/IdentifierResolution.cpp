#include "sparkc/frontend/semantic/IdentifierResolution.h"

IdentifierResolution::IdentifierResolution(
    Allocator& astAllocator,
    SymbolTable& symbolTable,
    TypeTable& typeTable,
    IdentifierGen& idGen
)
    : astAllocator(astAllocator)
    , idGen(idGen)
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
            varDecl->setInitializer(
                resolveExp(varDecl->getInitializer())
            );
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
        it->setExpression(resolveExp(it->getExpression()));
        break;
    }

    case AstStatement::Kind::Expression: {
        auto* it = (AstExpressionStatement*) st;
        it->setExpression(resolveExp(it->getExpression()));
        break;
    }

    case AstStatement::Kind::If: {
        auto* it = (AstIfStatement*) st;
        it->setCondition(resolveExp(it->getCondition()));
        resolve(it->getTrueBranch());

        if (auto* falseBranch = it->getFalseBranch()) {
            resolve(falseBranch);
        }
        break;
    }

    case AstStatement::Kind::While: {
        auto* it = (AstWhileStatement*) st;
        it->setCondition(resolveExp(it->getCondition()));
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

AstExp* IdentifierResolution::resolveExp(AstExp* exp) {
    switch (exp->kind) {
    case AstExp::Kind::Constant:
        return exp;

    case AstExp::Kind::Binary: {
        auto* it = (AstBinaryExp*) exp;
        it->setLeft(resolveExp(it->getLeft()));
        it->setRight(resolveExp(it->getRight()));
        break;
    }

    case AstExp::Kind::Var: {
        auto* it = (AstVar*) exp;
        it->setId(scope.get(it->getId(), ScopeItem::Kind::Var).id);
        break;
    }

    case AstExp::Kind::Assignment: {
        auto* it = (AstAssignment*) exp;
        it->setVar(resolveExp(it->getVar()));
        it->setExp(resolveExp(it->getExp()));
        break;
    }

    case AstExp::Kind::FunCall: {
        auto* it = (AstFunCall*) exp;
        if (scope.isDeclared(it->getFunName(), ScopeItem::Kind::Struct)) {
            auto* structInit = funCallToStructInit(it);
            resolve(structInit->getArgs());
            return structInit;
        }

        checkDeclaration(it->getFunName(), ScopeItem::Kind::Func);
        resolve(it->getArgs());
        break;
    }

    case AstExp::Kind::Cast: {
        auto* it = (AstCast*) exp;
        it->setExp(resolveExp(it->getExp()));
        break;
    }

    case AstExp::Kind::Dereference: {
        auto* it = (AstDereference*) exp;
        it->setExp(resolveExp(it->getExp()));
        break;
    }

    case AstExp::Kind::AddrOf: {
        auto* it = (AstAddrOf*) exp;
        it->setExp(resolveExp(it->getExp()));
        break;
    }

    case AstExp::Kind::Dot: {
        auto* it = (AstDot*) exp;
        it->setFrom(resolveExp(it->getFrom()));
        break;
    }

    case AstExp::Kind::StructInit: {
        sparkError("IdentifierResolution", "Unreachable branch");
        break;
    }

    default:
        sparkError("IdentifierResolution", "Unknown AstExp: %d", exp->kind);
        throw "";
    }

    return exp;
}

void IdentifierResolution::resolve(BoundArray<AstExp*>& array) {
    size_t sz = array.size();
    for (size_t i = 0; i < sz; i++) {
        auto* exp = array[i];
        array[i] = resolveExp(exp);
    }
}

void IdentifierResolution::checkDeclaration(StringRef name, ScopeItem::Kind kind) {
    scope.get(name, kind);
}

AstStructInit* IdentifierResolution::funCallToStructInit(AstFunCall* call) {
    return astAllocator.create<AstStructInit>(call->getFunName(), call->getArgs());
}
