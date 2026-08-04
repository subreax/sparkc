#include "SkrEmitter.h"

static SkrBinary::Operator binaryOpOf(AstBinaryExp::Operator astOp);
static SkrBranch::Operator branchOpOf(AstBinaryExp::Operator astOp);
static SkrBranch::Operator invertedBranchOpOf(AstBinaryExp::Operator astOp);
static bool isLogicalBin(AstExp* exp);

static std::vector<SkrInstruction*>& operator+=(
    std::vector<SkrInstruction*>& out,
    SkrInstruction* instr
) {
    out.emplace_back(instr);
    return out;
}

SkrFunction* SkrEmitter::emit(
    AstFunction* func,
    SkrFactory& factory,
    SymbolTable& symbolTable,
    TypeTable& typeTable,
    IdentifierGen& idGen,
    LabelGen& labelGen,
    std::vector<SkrInstruction*>& buf
) {
    return SkrEmitter(factory, idGen, labelGen, symbolTable, typeTable, buf)
        .emit(func);
}

SkrEmitter::SkrEmitter(
    SkrFactory& factory,
    IdentifierGen& idGen,
    LabelGen& labelGen,
    SymbolTable& symbolTable,
    TypeTable& typeTable,
    std::vector<SkrInstruction*>& out
)
    : skrf(factory)
    , symbolTable(symbolTable)
    , typesf(symbolTable.getTypeFactory())
    , typeTable(typeTable)
    , idGen(idGen)
    , labelGen(labelGen)
    , out(out) { }

SkrFunction* SkrEmitter::emit(AstFunction* func) {
    funName = func->getName();

    const auto& astParams = func->getParams();
    std::vector<SkrVar*> skrParams;
    skrParams.reserve(astParams.size());
    for (size_t i = 0; i < astParams.size(); i++) {
        auto* astParam = astParams[i];
        skrParams.emplace_back(skrf.var(astParam->getId()));
    }

    funcRetVal = createVar("retval", func->getReturnType());
    retLabel = labelGen.uniquePrivate("ret");
    emit(func->getBlock());
    out += skrf.label(retLabel);

    return skrf.function(
        func->getName(),
        skrParams,
        out,
        funcRetVal
    );
}

void SkrEmitter::emit(const AstBlock* block) {
    for (auto* item : block->getItems()) {
        emit(item);
    }
}

void SkrEmitter::emit(AstBlockItem* blockItem) {
    switch (blockItem->kind) {
    case AstBlockItem::Kind::Declaration:
        emit(((AstDeclBlockItem*) blockItem)->getDeclaration());
        return;
    case AstBlockItem::Kind::Statement:
        emit(((AstStatementBlockItem*) blockItem)->getStatement());
        return;
    default:
        sparkError("SkrEmitter", "Unknown AstBlockItem: %d", blockItem->kind);
    }
}

void SkrEmitter::emit(AstDeclaration* decl) {
    switch (decl->kind) {
    case AstDeclaration::Kind::Var:
        emit((AstVarDeclaration*) decl);
        return;
    default:
        sparkError("SkrEmitter", "Unknown AstDeclaration: %d", decl->kind);
    }
}

void SkrEmitter::emit(AstVarDeclaration* decl) {
    auto* initializer = decl->getInitializer();
    if (initializer == nullptr)
        return;
    emit(initializer, skrf.var(decl->getId()));
}

void SkrEmitter::emit(AstStatement* st) {
    switch (st->kind) {
    case AstStatement::Kind::Return: emit((AstReturnStatement*) st); return;
    case AstStatement::Kind::Expression: emit((AstExpressionStatement*) st); return;
    case AstStatement::Kind::If: emit((AstIfStatement*) st); return;
    case AstStatement::Kind::While: emit((AstWhileStatement*) st); return;
    case AstStatement::Kind::Compound: emit((AstCompoundStatement*) st); return;
    default: sparkError("SkrEmitter", "Unknown AstStatement: %d", st->kind);
    }
}

void SkrEmitter::emit(AstReturnStatement* st) {
    auto* retVal = emitAndConvert(st->getExpression(), funcRetVal);
    if (retVal != funcRetVal)
        out += skrf.copy(funcRetVal, retVal);
    out += skrf.jump(retLabel);
}

void SkrEmitter::emit(AstExpressionStatement* st) { emit(st->getExpression()); }

void SkrEmitter::emit(AstIfStatement* st) {
    auto ifFalseLabel = labelGen.uniquePrivate("false");
    emitBranchInverted(st->getCondition(), ifFalseLabel);
    emit(st->getTrueBranch());
    if (auto* falseBranch = st->getFalseBranch()) {
        auto endLabel = labelGen.uniquePrivate("end");
        out += skrf.jump(endLabel);
        out += skrf.label(ifFalseLabel);
        emit(falseBranch);
        out += skrf.label(endLabel);
        return;
    }
    out += skrf.label(ifFalseLabel);
}

void SkrEmitter::emit(AstWhileStatement* st) {
    auto startLabel = labelGen.uniquePrivate("start");
    auto endLabel = labelGen.uniquePrivate("end");
    out += skrf.label(startLabel);
    emitBranchInverted(st->getCondition(), endLabel);
    emit(st->getStatement());
    out += skrf.jump(startLabel);
    out += skrf.label(endLabel);
}

void SkrEmitter::emit(AstCompoundStatement* st) { emit(st->getBlock()); }

void SkrEmitter::emitBranch(AstExp* exp, StringRef trueLabel) {
    emitBranch(exp, trueLabel, false);
}

void SkrEmitter::emitBranchInverted(AstExp* exp, StringRef falseLabel) {
    emitBranch(exp, falseLabel, true);
}

void SkrEmitter::emitBranch(AstExp* exp, StringRef label, bool invert) {
    if (isLogicalBin(exp) && exp->type->kind == SymbolType::Kind::Integer) {
        AstBinaryExp* binExp = (AstBinaryExp*) exp;
        auto* left = emitAndConvert(binExp->getLeft());
        SkrBranch::Operator skrOp;
        if (invert) {
            skrOp = invertedBranchOpOf(binExp->getOperator());
        }
        else {
            skrOp = branchOpOf(binExp->getOperator());
        }
        auto* right = emitAndConvert(binExp->getRight());
        auto* branch = skrf.branch(left, skrOp, right, label);
        out += branch;
    }
    else {
        auto* res = emitAndConvert(exp);
        SkrBranch::Operator skrOp;
        if (invert) {
            skrOp = SkrBranch::Operator::Equals;
        }
        else {
            skrOp = SkrBranch::Operator::NotEquals;
        }
        auto* branch = skrf.branch(res, skrOp, skrf.constant(0), label);
        out += branch;
    }
}

SkrExpRes SkrEmitter::emit(AstExp* exp, SkrVar* dst) {
    switch (exp->kind) {
    case AstExp::Kind::Constant:
        return emitConstant((AstConstantExp*) exp, dst);

    case AstExp::Kind::AddrOf:
        return emitAddrOf((AstAddrOf*) exp, dst);

    case AstExp::Kind::Binary:
        return emitBinary((AstBinaryExp*) exp, dst);

    case AstExp::Kind::Var:
        return emitVar((AstVar*) exp, dst);

    case AstExp::Kind::Assignment:
        return emitAssignment((AstAssignment*) exp);

    case AstExp::Kind::FunCall:
        return emitFunCall((AstFunCall*) exp, dst);

    case AstExp::Kind::Cast:
        return emitCast((AstCast*) exp, dst);

    case AstExp::Kind::Dot:
        return emitDot((AstDot*) exp);

    case AstExp::Kind::StructInit:
        return emitStructInit((AstStructInit*) exp, dst);

    default:
        sparkError("SkrEmitter", "Unknown AstExp: %d", exp->kind);
        return SkrExpRes::val(nullptr);
    }
}

SkrExpRes SkrEmitter::emitConstant(AstConstantExp* exp, SkrVar* dst) {
    auto* constant = skrf.constant(exp->getValue());
    if (dst == nullptr) {
        return SkrExpRes::val(constant);
    }
    out += skrf.copy(dst, constant);
    return SkrExpRes::val(dst);
}

SkrExpRes SkrEmitter::emitAddrOf(AstAddrOf* exp, SkrVar* dst) {
    SkrExpRes var = emit(exp->getExp());
    if (dst == nullptr)
        dst = createVar("addr", typesf.pointer(getType(var.get())));
    out += skrf.getAddr(dst, var.get()->toSkrVar());
    return SkrExpRes::val(dst);
}

SkrExpRes SkrEmitter::emitVar(AstVar* exp, SkrVar* dst) {
    auto* var = skrf.var(exp->getId());
    if (dst == nullptr)
        return SkrExpRes::val(var);
    out += skrf.copy(dst, var);
    return SkrExpRes::val(dst);
}

SkrExpRes SkrEmitter::emitAssignment(AstAssignment* exp) {
    SkrExpRes left = emit(exp->getVar());
    if (left.kind != SkrExpRes::Kind::Field) {
        return SkrExpRes::val(emitAndConvert(exp->getExp(), left.get()->toSkrVar()));
    }
    SkrValue* right = emitAndConvert(exp->getExp());
    out += skrf.copyToOffset(left.getBase(), left.getOffset(), right);
    return SkrExpRes::val(right);
}

SkrExpRes SkrEmitter::emitCast(AstCast* exp, SkrVar* dst) {
    auto* targetType = exp->type;
    SkrValue* srcVal = emitAndConvert(exp->getExp());
    if (dst == nullptr) {
        dst = createVar("cast", targetType);
    }

    if (getTypeKind(srcVal) == SymbolType::Kind::Integer && targetType->kind == SymbolType::Kind::Float) {
        out += skrf.int2Float(dst, srcVal);
    }
    else if (getTypeKind(srcVal) == SymbolType::Kind::Float && targetType->kind == SymbolType::Kind::Integer) {
        out += skrf.float2Int(dst, srcVal);
    }
    else {
        sparkError("SkrEmitter", "Failed to cast expression");
    }
    return SkrExpRes::val(dst);
}

SkrExpRes SkrEmitter::emitDot(AstDot* exp) {
    const auto& field = typeTable.getField(
        getStructTag(exp->getFrom()->type),
        getFieldId(exp->getField())
    );
    auto inner = emit(exp->getFrom());
    return SkrExpRes::field(inner.getBase(), inner.getOffset() + field.offset);
}

StringRef SkrEmitter::getFieldId(AstExp* exp) {
    switch (exp->kind) {
    case AstExp::Kind::Var:
        return ((AstVar*) exp)->getId();
    default:
        sparkError("SkrEmitter", "Field is not a var: %d", exp->kind);
        return StringRef::nullInstance();
    }
}

SkrExpRes SkrEmitter::emitBinary(AstBinaryExp* exp, SkrVar* dst) {
    SkrValue* result;
    switch (exp->getOperator()) {
    case AstBinaryExp::Operator::And: {
        auto falseLabel = labelGen.uniquePrivate("and_false");
        auto endLabel = labelGen.uniquePrivate("and_end");

        if (dst == nullptr) {
            dst = createVar("and", typesf.int_());
        }

        emitBranchInverted(exp->getLeft(), falseLabel);
        emitBranchInverted(exp->getRight(), falseLabel);
        // true
        out += skrf.copy(dst, skrf.constant(1));
        out += skrf.jump(endLabel);

        // false
        out += skrf.label(falseLabel);
        out += skrf.copy(dst, skrf.constant(0));

        out += skrf.label(endLabel);
        result = dst;
        break;
    }
    case AstBinaryExp::Operator::Or: {
        auto trueLabel = labelGen.uniquePrivate("or_true");
        auto endLabel = labelGen.uniquePrivate("or_end");

        if (dst == nullptr) {
            dst = createVar("or", typesf.int_());
        }

        emitBranch(exp->getLeft(), trueLabel);
        emitBranch(exp->getRight(), trueLabel);
        // false
        out += skrf.copy(dst, skrf.constant(0));
        out += skrf.jump(endLabel);

        // true
        out += skrf.label(trueLabel);
        out += skrf.copy(dst, skrf.constant(1));

        out += skrf.label(endLabel);
        result = dst;
        break;
    }
    default: {
        SkrValue* left = emitAndConvert(exp->getLeft());
        auto op = binaryOpOf(exp->getOperator());
        SkrValue* right = emitAndConvert(exp->getRight());
        if (dst == nullptr) {
            dst = createVar(funName, getType(left));
        }
        out += skrf.binary(dst, left, op, right);
        result = dst;
    }
    }
    return SkrExpRes::val(result);
}

SkrExpRes SkrEmitter::emitFunCall(AstFunCall* call, SkrVar* dst) {
    auto astArgs = call->getArgs();
    std::vector<SkrValue*> skrArgs;
    skrArgs.reserve(astArgs.size());
    for (size_t i = 0; i < astArgs.size(); i++) {
        skrArgs.emplace_back(emitAndConvert(astArgs[i]));
    }

    if (dst == nullptr) {
        dst = createVar(call->getFunName(), "r", call->type);
    }
    auto* skrCall = skrf.funCall(call->getFunName(), skrArgs, dst);
    out += skrCall;
    return SkrExpRes::val(dst);
}

SkrExpRes SkrEmitter::emitStructInit(AstStructInit* it, SkrVar* dst) {
    auto tag = it->getTag();
    auto args = it->getArgs();

    if (dst == nullptr) {
        dst = createVar(tag, it->type);
    }

    const auto& fields = typeTable.get(tag);
    for (size_t i = 0; i < args.size(); i++) {
        auto* arg = emitAndConvert(args[i]);
        int offset = fields[i].offset;
        auto* instr = skrf.copyToOffset(dst, offset, arg);
        out += instr;
    }
    return SkrExpRes::val(dst);
}

SkrValue* SkrEmitter::emitAndConvert(AstExp* exp, SkrVar* dst) {
    auto res = emit(exp, dst);

    switch (res.kind) {
    case SkrExpRes::Kind::Val:
        return res.get();

    case SkrExpRes::Kind::Field:
        if (dst == nullptr) {
            dst = createVar("field", exp->type);
        }
        out += skrf.copyFromOffset(dst, res.getBase(), res.getOffset());
        return dst;

    default:
        sparkError("SkrEmitter", "Unknown SkrExpRes kind: %d", res.kind);
        return nullptr;
    }
}

SymbolType::Kind SkrEmitter::getTypeKind(SkrValue* value) {
    return getType(value)->kind;
}

SymbolType* SkrEmitter::getType(SkrValue* value) {
    switch (value->kind) {
    case SkrValue::Kind::Const:
        return value->toSkrConst()->getConst()->type;
    case SkrValue::Kind::Var:
        return symbolTable.get(value->toSkrVar()->getId());
    default:
        sparkError("SkrEmitter", "Unknown SkrValue kind: %d", value->kind);
        return nullptr;
    }
}

SymbolType* SkrEmitter::dereferenceType(SymbolType* t) {
    if (t->kind == SymbolType::Kind::Pointer) {
        auto* it = (SymbolPointerType*) t;
        return it->getVarType();
    }
    sparkError("SkrEmitter", "Failed to dereference type");
    return t;
}

SymbolType* SkrEmitter::dereferenceType(SkrValue* value) {
    return dereferenceType(getType(value));
}

StringRef SkrEmitter::getStructTag(SymbolType* type) {
    switch (type->kind) {
    case SymbolType::Kind::Structure:
        return ((SymbolStructureType*) type)->getTag();
    case SymbolType::Kind::Pointer:
        return getStructTag(((SymbolPointerType*) type)->getVarType());
    default:
        sparkError("SkrEmitter", "Expected a structure, but found kind %d", type->kind);
        return StringRef::nullInstance();
    }
}

SkrVar* SkrEmitter::createVar(StringRef name, SymbolType* type) {
    auto id = idGen.unique(name);
    symbolTable.declareVar(id, type);
    return skrf.var(id);
}

SkrVar* SkrEmitter::createVar(const char* name, SymbolType* type) {
    return createVar(StringRef::cstr(name), type);
}

SkrVar* SkrEmitter::createVar(StringRef name, const char* suffix, SymbolType* type) {
    auto id = idGen.unique(name, suffix);
    symbolTable.declareVar(id, type);
    return skrf.var(id);
}

static SkrBinary::Operator binaryOpOf(AstBinaryExp::Operator astOp) {
    switch (astOp) {
    case AstBinaryExp::Operator::Plus: return SkrBinary::Operator::Plus;
    case AstBinaryExp::Operator::Minus: return SkrBinary::Operator::Minus;
    case AstBinaryExp::Operator::Mul: return SkrBinary::Operator::Mul;
    case AstBinaryExp::Operator::Div: return SkrBinary::Operator::Div;
    case AstBinaryExp::Operator::Rem: return SkrBinary::Operator::Rem;
    case AstBinaryExp::Operator::Equals: return SkrBinary::Operator::Equals;
    case AstBinaryExp::Operator::NotEquals: return SkrBinary::Operator::NotEquals;
    case AstBinaryExp::Operator::LessThan: return SkrBinary::Operator::LessThan;
    case AstBinaryExp::Operator::LessOrEqual: return SkrBinary::Operator::LessOrEqual;
    case AstBinaryExp::Operator::GreaterThan: return SkrBinary::Operator::GreaterThan;
    case AstBinaryExp::Operator::GreaterOrEqual: return SkrBinary::Operator::GreaterOrEqual;
    default:
        sparkError(
            "SkrBinary",
            "Can't map AstBinaryExp::Operator to SkrBinary::Operator: %d",
            astOp
        );
        return SkrBinary::Operator::Plus;
    }
}

static SkrBranch::Operator branchOpOf(AstBinaryExp::Operator astOp) {
    switch (astOp) {
    case AstBinaryExp::Operator::Equals: return SkrBranch::Operator::Equals;
    case AstBinaryExp::Operator::NotEquals: return SkrBranch::Operator::NotEquals;
    case AstBinaryExp::Operator::LessThan: return SkrBranch::Operator::LessThan;
    case AstBinaryExp::Operator::LessOrEqual: return SkrBranch::Operator::LessOrEqual;
    case AstBinaryExp::Operator::GreaterThan: return SkrBranch::Operator::GreaterThan;
    case AstBinaryExp::Operator::GreaterOrEqual: return SkrBranch::Operator::GreaterOrEqual;
    default:
        sparkError(
            "SkrBinary",
            "Can't map AstBinaryExp::Operator to SkrBranch::Operator: %d",
            astOp
        );
        return SkrBranch::Operator::Equals;
    }
}

static SkrBranch::Operator invertedBranchOpOf(AstBinaryExp::Operator astOp) {
    switch (astOp) {
    case AstBinaryExp::Operator::Equals: return SkrBranch::Operator::NotEquals;
    case AstBinaryExp::Operator::NotEquals: return SkrBranch::Operator::Equals;
    case AstBinaryExp::Operator::LessThan: return SkrBranch::Operator::GreaterOrEqual;
    case AstBinaryExp::Operator::LessOrEqual: return SkrBranch::Operator::GreaterThan;
    case AstBinaryExp::Operator::GreaterThan: return SkrBranch::Operator::LessOrEqual;
    case AstBinaryExp::Operator::GreaterOrEqual: return SkrBranch::Operator::LessThan;
    default:
        sparkError(
            "SkrBinary",
            "Can't map inv AstBinaryExp::Operator to SkrBranch::Operator: %d",
            astOp
        );
        return SkrBranch::Operator::Equals;
    }
}

static bool isLogicalBin(AstExp* exp) {
    if (exp->kind != AstExp::Kind::Binary) {
        return false;
    }

    auto* binExp = (AstBinaryExp*) exp;
    auto op = binExp->getOperator();
    switch (binExp->getOperator()) {
    case AstBinaryExp::Operator::Equals:
    case AstBinaryExp::Operator::NotEquals:
    case AstBinaryExp::Operator::LessThan:
    case AstBinaryExp::Operator::LessOrEqual:
    case AstBinaryExp::Operator::GreaterThan:
    case AstBinaryExp::Operator::GreaterOrEqual:
        return true;

    default:
        return false;
    }
}
