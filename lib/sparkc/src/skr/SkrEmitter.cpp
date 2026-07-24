#include "SkrEmitter.h"

SkrFunction* SkrEmitter::emit(
    AstFunction* func,
    Allocator& allocator,
    SymbolTable& symbolTable,
    TypeTable& typeTable,
    IdentifierGen& idGen,
    LabelGen& labelGen,
    std::vector<SkrInstruction*>& buf
) {
    return SkrEmitter(allocator, idGen, labelGen, symbolTable, typeTable, buf)
        .emit(func);
}

SkrEmitter::SkrEmitter(
    Allocator& allocator,
    IdentifierGen& idGen,
    LabelGen& labelGen,
    SymbolTable& symbolTable,
    TypeTable& typeTable,
    std::vector<SkrInstruction*>& out
)
    : allocator(allocator)
    , symbolTable(symbolTable)
    , typeTable(typeTable)
    , idGen(idGen)
    , labelGen(labelGen)
    , out(out) { }

SkrFunction* SkrEmitter::emit(AstFunction* func) {
    funName = func->getName();

    const auto& astParams = func->getParams();
    auto skrParams = BoundArray<SkrVar*>::create(astParams.size(), allocator);
    for (size_t i = 0; i < astParams.size(); i++) {
        auto* astParam = astParams[i];
        auto* skrParam = allocator.create<SkrVar>(astParam->getId());
        skrParams[i] = skrParam;
    }

    funcRetVal = createVar("retval", func->getReturnType());
    retLabel = labelGen.uniquePrivate("ret");
    emit(func->getBlock());
    out.emplace_back(allocator.create<SkrLabel>(retLabel));

    auto baInstructions = BoundArray<SkrInstruction*>::fromVector(out, allocator);
    return allocator.create<SkrFunction>(
        func->getName(),
        skrParams,
        baInstructions,
        funcRetVal
    );
}

void SkrEmitter::emit(const AstBlock* block) {
    for (auto* item : block->getItems()) {
        emit(item);
    }
}

void SkrEmitter::emit(AstBlockItem* blockItem) {
    auto kind = blockItem->kind;
    if (kind == AstBlockItem::Kind::Declaration) {
        emit(((AstDeclBlockItem*) blockItem)->getDeclaration());
    }
    else if (kind == AstBlockItem::Kind::Statement) {
        emit(((AstStatementBlockItem*) blockItem)->getStatement());
    }
    else {
        sparkError("SkrEmitter", "Unknown AstBlockItem: %d", kind);
    }
}

void SkrEmitter::emit(AstDeclaration* decl) {
    if (decl->kind == AstDeclaration::Kind::Var) {
        auto* it = (AstVarDeclaration*) decl;
        auto* initializer = it->getInitializer();
        if (initializer == nullptr) {
            return;
        }

        auto* skrVar = allocator.create<SkrVar>(it->getId());
        emit(initializer, skrVar);
    }
    else {
        sparkError("SkrEmitter", "Unknown AstDeclaration: %d", decl->kind);
    }
}

void SkrEmitter::emit(AstStatement* st) {
    auto kind = st->kind;
    if (kind == AstStatement::Kind::Return) {
        auto* it = (AstReturnStatement*) st;
        auto* retVal = emitAndConvert(it->getExpression(), funcRetVal);
        if (retVal != funcRetVal) {
            out.emplace_back(allocator.create<SkrCopy>(funcRetVal, retVal));
        }
        out.emplace_back(allocator.create<SkrJump>(retLabel));
    }
    else if (kind == AstStatement::Kind::Expression) {
        auto* it = (AstExpressionStatement*) st;
        emit(it->getExpression());
    }
    else if (kind == AstStatement::Kind::If) {
        auto* it = (AstIfStatement*) st;
        auto ifFalseLabel = labelGen.uniquePrivate("false");
        emitInvertBranch(it->getCondition(), ifFalseLabel);
        emit(it->getTrueBranch());

        auto* falseBranch = it->getFalseBranch();
        if (falseBranch != nullptr) {
            auto endLabel = labelGen.uniquePrivate("end");
            out.emplace_back(allocator.create<SkrJump>(endLabel));
            out.emplace_back(allocator.create<SkrLabel>(ifFalseLabel));
            emit(falseBranch);
            out.emplace_back(allocator.create<SkrLabel>(endLabel));
        }
        else {
            out.emplace_back(allocator.create<SkrLabel>(ifFalseLabel));
        }
    }
    else if (kind == AstStatement::Kind::While) {
        auto* it = (AstWhileStatement*) st;
        auto startLabel = labelGen.uniquePrivate("start");
        auto endLabel = labelGen.uniquePrivate("end");
        out.emplace_back(allocator.create<SkrLabel>(startLabel));
        emitInvertBranch(it->getCondition(), endLabel);
        emit(it->getStatement());
        out.emplace_back(allocator.create<SkrJump>(startLabel));
        out.emplace_back(allocator.create<SkrLabel>(endLabel));
    }
    else if (kind == AstStatement::Kind::Compound) {
        auto* it = (AstCompoundStatement*) st;
        emit(it->getBlock());
    }
    else {
        sparkError("SkrEmitter", "Unknown AstStatement: %d", kind);
    }
}

void SkrEmitter::emitBranch(AstExp* exp, StringRef trueLabel) {
    emitBranch(exp, trueLabel, false);
}

void SkrEmitter::emitInvertBranch(AstExp* exp, StringRef falseLabel) {
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
        auto* branch = allocator.create<SkrBranch>(left, skrOp, right, label);
        out.emplace_back(branch);
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
        auto* branch = allocator.create<SkrBranch>(res, skrOp, getSkrIntConst(0), label);
        out.emplace_back(branch);
    }
}

SkrExpRes SkrEmitter::emit(AstExp* exp, SkrVar* dst) {
    auto kind = exp->kind;
    if (kind == AstExp::Kind::Constant) {
        auto* it = (AstConstantExp*) exp;
        auto* c = getSkrConst(it->getValue());
        if (dst) {
            out.emplace_back(allocator.create<SkrCopy>(dst, c));
        }
        return SkrExpRes::val(c);
    }
    /* else if (kind == AstExp::Kind::Dereference) {
        auto* it = (AstDereference*) exp;
        SkrValue* innerRes = emitAndConvert(it->getExpression());
        return SkrExpRes::ptr(innerRes, 0);
    } */
    else if (kind == AstExp::Kind::AddrOf) {
        auto* it = (AstAddrOf*) exp;
        SkrExpRes var = emit(it->getExp());
        auto* toType = symbolTable.getTypeFactory().pointer(getType(var.get()));
        if (dst == nullptr) {
            dst = createVar("addr", toType);
        }
        out.emplace_back(
            allocator.create<SkrGetAddr>(dst, var.get()->toSkrVar())
        );
        return SkrExpRes::val(dst);
    }
    else if (kind == AstExp::Kind::Binary) {
        auto* res = emitBinary((AstBinaryExp*) exp, dst);
        return SkrExpRes::val(res);
    }
    else if (kind == AstExp::Kind::Var) {
        auto* var = allocator.create<SkrVar>(((AstVar*) exp)->getId());
        return SkrExpRes::val(var);
    }
    else if (kind == AstExp::Kind::Assignment) {
        auto* ass = (AstAssignment*) exp;
        SkrExpRes left = emit(ass->getVar());
        SkrValue* right = emitAndConvert(ass->getExp());
        /* if (left.kind == SkrExpRes::Kind::Ptr) {
            out.emplace_back(allocator.create<SkrStore>(left.getBase(),
        left.getOffset(), right)); return left;
        }
        else */
        if (left.kind == SkrExpRes::Kind::Field) {
            out.emplace_back(allocator.create<SkrCopyToOffset>(
                left.getBase(),
                left.getOffset(),
                right
            ));
            return SkrExpRes::val(right);
        }
        else {
            out.emplace_back(
                allocator.create<SkrCopy>(left.get()->toSkrVar(), right)
            );
            return SkrExpRes::val(right);
        }
    }
    else if (kind == AstExp::Kind::FunCall) {
        auto* res = emitFunCall((AstFunCall*) exp, dst);
        return SkrExpRes::val(res);
    }
    else if (kind == AstExp::Kind::Cast) {
        auto* it = (AstCast*) exp;
        auto targetType = exp->type;
        SkrValue* srcVal = emitAndConvert(it->getExp());
        if (dst == nullptr) {
            dst = createVar("cast", targetType);
        }

        if (getTypeKind(srcVal) == SymbolType::Kind::Integer && targetType->kind == SymbolType::Kind::Float) {
            out.emplace_back(allocator.create<SkrInt2Float>(dst, srcVal));
        }
        else if (
            getTypeKind(srcVal) == SymbolType::Kind::Float && targetType->kind == SymbolType::Kind::Integer
        ) {
            out.emplace_back(allocator.create<SkrFloat2Int>(dst, srcVal));
        }
        else {
            sparkError("SkrEmitter", "Failed to cast expression");
        }
        return SkrExpRes::val(dst);
    }
    else if (kind == AstExp::Kind::Dot) {
        auto* it = (AstDot*) exp;

        StringRef field = getFieldId(it->getField());
        StringRef tag = getStructTag(it->getFrom()->type);
        int offset = typeTable.getField(tag, field).offset;

        auto inner = emit(it->getFrom());
        /* if (inner.kind == SkrExpRes::Kind::Ptr) {
            return SkrExpRes::ptr(inner.get(), inner.getOffset() + offset);
        } else { */
        return SkrExpRes::field(inner.getBase(), inner.getOffset() + offset);
        // }
    }
    else if (kind == AstExp::Kind::StructInit) {
        auto* res = emitStructInit((AstStructInit*) exp, dst);
        return SkrExpRes::val(res);
    }
    else {
        sparkError("SkrEmitter", "Unknown AstExp: %d", kind);
        return SkrExpRes::val(nullptr);
    }
}

StringRef SkrEmitter::getFieldId(AstExp* exp) {
    if (exp->kind == AstExp::Kind::Var) {
        return ((AstVar*) exp)->getId();
    }
    sparkError("SkrEmitter", "Field is not a var: %d", exp->kind);
    return StringRef::nullInstance();
}

SkrValue* SkrEmitter::emitBinary(AstBinaryExp* exp, SkrVar* dst) {
    auto astOp = exp->getOperator();
    if (astOp == AstBinaryExp::Operator::And) {
        auto falseLabel = labelGen.uniquePrivate("and_false");
        auto endLabel = labelGen.uniquePrivate("and_end");

        SkrVar* result = createVar("and", symbolTable.getTypeFactory().int_());
        emitInvertBranch(exp->getLeft(), falseLabel);
        emitInvertBranch(exp->getRight(), falseLabel);
        // true
        out.emplace_back(allocator.create<SkrCopy>(result, getSkrIntConst(1)));
        out.emplace_back(allocator.create<SkrJump>(endLabel));

        // false
        out.emplace_back(allocator.create<SkrLabel>(falseLabel));
        out.emplace_back(allocator.create<SkrCopy>(result, getSkrIntConst(0)));

        out.emplace_back(allocator.create<SkrLabel>(endLabel));
        return result;
    }
    else if (astOp == AstBinaryExp::Operator::Or) {
        auto trueLabel = labelGen.uniquePrivate("or_true");
        auto endLabel = labelGen.uniquePrivate("or_end");

        SkrVar* result = createVar("or", symbolTable.getTypeFactory().int_());
        emitBranch(exp->getLeft(), trueLabel);
        emitBranch(exp->getRight(), trueLabel);
        // false
        out.emplace_back(allocator.create<SkrCopy>(result, getSkrIntConst(0)));
        out.emplace_back(allocator.create<SkrJump>(endLabel));

        // true
        out.emplace_back(allocator.create<SkrLabel>(trueLabel));
        out.emplace_back(allocator.create<SkrCopy>(result, getSkrIntConst(1)));

        out.emplace_back(allocator.create<SkrLabel>(endLabel));
        return result;
    }
    else {
        SkrValue* left = emitAndConvert(exp->getLeft());
        auto op = binaryOpOf(exp->getOperator());
        SkrValue* right = emitAndConvert(exp->getRight());
        if (dst == nullptr) {
            dst = createVar(funName, getType(left));
        }
        out.emplace_back(allocator.create<SkrBinary>(dst, left, op, right));
        return dst;
    }
}

SkrValue* SkrEmitter::emitFunCall(AstFunCall* call, SkrVar* dst) {
    auto astArgs = call->getArgs();
    auto skrArgs = BoundArray<SkrValue*>::create(astArgs.size(), allocator);
    for (size_t i = 0; i < astArgs.size(); i++) {
        skrArgs[i] = emitAndConvert(astArgs[i]);
    }

    if (dst == nullptr) {
        dst = createVar(call->getFunName(), "r", call->type);
    }
    auto* skrCall = allocator.create<SkrFunCall>(call->getFunName(), skrArgs, dst);
    out.emplace_back(skrCall);
    return dst;
}

SkrValue* SkrEmitter::emitStructInit(AstStructInit* it, SkrVar* dst) {
    auto tag = it->getTag();
    auto args = it->getArgs();

    if (dst == nullptr) {
        dst = createVar(tag, it->type);
    }

    const auto& fields = typeTable.get(tag);
    for (size_t i = 0; i < args.size(); i++) {
        auto* arg = emitAndConvert(args[i]);
        int offset = fields[i].offset;
        auto* instr = allocator.create<SkrCopyToOffset>(dst, offset, arg);
        out.emplace_back(instr);
    }
    return dst;
}

SkrValue* SkrEmitter::emitAndConvert(AstExp* exp, SkrVar* dst) {
    auto res = emit(exp, dst);
    if (res.kind == SkrExpRes::Kind::Val) {
        return res.get();
    }
    /* else if (res.kind == SkrExpRes::Kind::Ptr) {
        SkrValue* tmpVar = createVar("deref", exp->type);
        out.emplace_back(allocator.create<SkrLoad>(tmpVar, res.get(),
    res.getOffset())); return tmpVar;
    } */
    else if (res.kind == SkrExpRes::Kind::Field) {
        if (dst == nullptr) {
            dst = createVar("field", exp->type);
        }
        out.emplace_back(allocator.create<SkrCopyFromOffset>(
            dst,
            res.getBase(),
            res.getOffset()
        ));
        return dst;
    }

    sparkError("SkrEmitter", "Unknown SkrExpRes kind: %d", res.kind);
    return nullptr;
}

SymbolType::Kind SkrEmitter::getTypeKind(SkrValue* value) {
    return getType(value)->kind;
}

SymbolType* SkrEmitter::getType(SkrValue* value) {
    if (value->isConst()) {
        return value->toSkrConst()->getConst()->type;
    }
    else if (value->isVar()) {
        return symbolTable.get(value->toSkrVar()->getId());
    }
    else {
        sparkError("SkrEmitter", "Unknown SkrValue kind");
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
    if (type->kind == SymbolType::Kind::Structure) {
        return ((SymbolStructureType*) type)->getTag();
    }
    else if (type->kind == SymbolType::Kind::Pointer) {
        return getStructTag(((SymbolPointerType*) type)->getVarType());
    }

    sparkError(
        "SkrEmitter",
        "Expected a structure, but found kind %d",
        type->kind
    );
    return StringRef::nullInstance();
}

SkrBinary::Operator SkrEmitter::binaryOpOf(AstBinaryExp::Operator astOp) {
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

SkrBranch::Operator SkrEmitter::branchOpOf(AstBinaryExp::Operator astOp) {
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

SkrBranch::Operator
SkrEmitter::invertedBranchOpOf(AstBinaryExp::Operator astOp) {
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

bool SkrEmitter::isLogicalBin(AstExp* exp) {
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

void SkrEmitter::removeUselessJumpToRet() {
    int i = out.size() - 1;
    for (; i >= 0; i--) {
        auto* skr = out[i];
        auto kind = skr->kind;
        if (kind == SkrInstruction::Kind::Label) {
            continue;
        }

        if (kind == SkrInstruction::Kind::Jump) {
            auto* jmp = (SkrJump*) skr;
            if (jmp->getLabel() == retLabel) {
                out.erase(out.begin() + i);
                break;
            }
            else {
                break;
            }
        }
        else {
            break;
        }
    }
}

SkrVar* SkrEmitter::createVar(StringRef name, SymbolType* type) {
    auto id = idGen.unique(name);
    symbolTable.declareVar(id, type);
    return allocator.create<SkrVar>(id);
}

SkrVar* SkrEmitter::createVar(const char* name, SymbolType* type) {
    return createVar(StringRef::cstr(name), type);
}

SkrVar*
SkrEmitter::createVar(StringRef name, const char* suffix, SymbolType* type) {
    auto id = idGen.unique(name, suffix);
    symbolTable.declareVar(id, type);
    return allocator.create<SkrVar>(id);
}

SkrConst* SkrEmitter::getSkrConst(Constant* c) {
    if (c->isInt()) {
        return getSkrIntConst(c->intValue());
    }
    else if (c->isFloat()) {
        auto* c1 = allocator.create<FloatConstant>(c->floatValue());
        return allocator.create<SkrConst>(c1);
    }
    sparkError("SkrEmitter", "Unknown Constant");
    return nullptr;
}

SkrConst* SkrEmitter::getSkrIntConst(int32_t v) {
    if (v == 0) {
        return SkrConst::getInt0();
    }
    else if (v == 1) {
        return SkrConst::getInt1();
    }
    else {
        return allocator.create<SkrConst>(allocator.create<IntConstant>(v));
    }
}
