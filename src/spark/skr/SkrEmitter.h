#pragma once
#include <vector>
#include "../common/Error.h"
#include "../common/alloc/LinearAllocator.h"
#include "../common/IdentifierGen.h"
#include "../common/LabelGen.h"
#include "../frontend/ast/exp/everything.h"
#include "../symbol/SymbolTable.h"
#include "../type/TypeTable.h"
#include "SkrFunction.h"
#include "instr/everything.h"
#include "value/SkrExpRes.h"

class SkrEmitter {
public:
    static SkrFunction* emit(AstFunction* func, Allocator& allocator, SymbolTable& symbolTable, TypeTable& typeTable, IdentifierGen& idGen, LabelGen& labelGen, std::vector<SkrInstruction*>& buf) {
        return SkrEmitter(allocator, idGen, labelGen, symbolTable, typeTable, buf).emit(func);
    }

private:
    SkrEmitter(Allocator& allocator, IdentifierGen& idGen, LabelGen& labelGen, SymbolTable& symbolTable, TypeTable& typeTable, std::vector<SkrInstruction*>& out) 
        : allocator(allocator)
        , symbolTable(symbolTable)
        , typeTable(typeTable)
        , idGen(idGen)
        , labelGen(labelGen)
        , out(out) {  }

    SkrFunction* emit(AstFunction* func) {
        funName = func->getName();

        const auto& astParams = func->getParams();
        auto skrParams = BoundArray<SkrVar*>::create(astParams.size(), allocator);
        for (size_t i = 0; i < astParams.size(); i++) {
            auto* astParam = astParams[i];
            auto* skrParam = allocator.create<SkrVar>(astParam->getId());
            skrParams[i] = skrParam;
        }

        funcRetVal = createVar("retval", func->getReturnType());
        retLabel = labelGen.uniqueInternal("ret");
        emit(func->getBlock());
        out.emplace_back(allocator.create<SkrLabel>(retLabel));

        auto baInstructions = BoundArray<SkrInstruction*>::fromVector(out, allocator);
        return allocator.create<SkrFunction>(func->getName(), skrParams, baInstructions, funcRetVal);
    }

    void emit(const AstBlock* block) {
        for (auto* item : block->getItems()) {
            emit(item);
        }
    }

    void emit(AstBlockItem* blockItem) {
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

    void emit(AstDeclaration* decl) {
        if (decl->kind == AstDeclaration::Kind::Var) {
            auto* it = (AstVarDeclaration*) decl;
            auto* initializer = it->getInitializer();
            if (it->getType()->kind != SymbolType::Kind::Structure && initializer != nullptr) {
                auto* initRes = emitAndConvert(initializer);
                auto* skrVar = allocator.create<SkrVar>(it->getId());
                out.emplace_back(allocator.create<SkrCopy>(skrVar, initRes));
            }
        }
        else {
            sparkError("SkrEmitter", "Unknown AstDeclaration: %d", decl->kind);
        }
    }

    void emit(AstStatement* st) {
        auto kind = st->kind;
        if (kind == AstStatement::Kind::Return) {
            auto* it = (AstReturnStatement*) st;
            auto* retVal = emitAndConvert(it->getExpression());
            out.emplace_back(allocator.create<SkrCopy>(funcRetVal, retVal));
            out.emplace_back(allocator.create<SkrJump>(retLabel));
        }
        else if (kind == AstStatement::Kind::Expression) {
            auto* it = (AstExpressionStatement*) st;
            emit(it->getExpression());
        }
        else if (kind == AstStatement::Kind::If) {
            auto* it = (AstIfStatement*) st;
            auto ifFalseLabel = labelGen.uniqueInternal("false");
            emitInvertBranch(it->getCondition(), ifFalseLabel);
            emit(it->getTrueBranch());

            auto* falseBranch = it->getFalseBranch();
            if (falseBranch != nullptr) {
                auto endLabel = labelGen.uniqueInternal("end");
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
            auto startLabel = labelGen.uniqueInternal("start");
            auto endLabel = labelGen.uniqueInternal("end");
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

    void emitBranch(AstExp* exp, StringRef trueLabel) {
        emitBranch(exp, trueLabel, false);
    }

    void emitInvertBranch(AstExp* exp, StringRef falseLabel) {
        emitBranch(exp, falseLabel, true);
    }

    void emitBranch(AstExp* exp, StringRef label, bool invert) {
        if (isLogicalBin(exp) && exp->type->kind == SymbolType::Kind::Integer) {
            AstBinaryExp* binExp = (AstBinaryExp*) exp;
            auto* left = emitAndConvert(binExp->getLeft());
            SkrBranch::Operator skrOp;
            if (invert) {
                skrOp = invertedBranchOpOf(binExp->getOperator());
            } else {
                skrOp = branchOpOf(binExp->getOperator());
            }
            auto* right = emitAndConvert(binExp->getRight());
            auto* branch = allocator.create<SkrBranch>(left, skrOp, right, label);
            out.emplace_back(branch);
        } else {
            auto* res = emitAndConvert(exp);
            SkrBranch::Operator skrOp;
            if (invert) {
                skrOp = SkrBranch::Operator::Equals;
            } else {
                skrOp = SkrBranch::Operator::NotEquals;
            }
            auto* branch = allocator.create<SkrBranch>(res, skrOp, getSkrConst(0), label);
            out.emplace_back(branch);
        }
    }

    SkrExpRes emit(AstExp* exp) {
        auto kind = exp->kind;
        if (kind == AstExp::Kind::Constant) {
            auto* it = (AstConstantExp*) exp;
            auto* c = getSkrConst(it->getValue());
            return SkrExpRes::val(c);
        }
        else if (kind == AstExp::Kind::Dereference) {
            auto* it = (AstDereference*) exp;
            SkrValue* innerRes = emitAndConvert(it->getExpression());
            return SkrExpRes::ptr(innerRes);
        }
        else if (kind == AstExp::Kind::AddrOf) {
            auto* it = (AstAddrOf*) exp;
            SkrExpRes var = emit(it->getExpression());
            auto* toType = symbolTable.getTypeAllocator().create<SymbolPointerType>(getType(var.get()));
            SkrVar* to = createVar("addr", toType);
            out.emplace_back(allocator.create<SkrGetAddr>(to, var.get()->toSkrVar()));
            return SkrExpRes::val(to);
        }
        else if (kind == AstExp::Kind::Binary) {
            auto* res = emitBinary((AstBinaryExp*) exp);
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
            if (left.kind == SkrExpRes::Kind::Ptr) {
                out.emplace_back(allocator.create<SkrStore>(left.get(), right));
                return left;
            }
            else if (left.kind == SkrExpRes::Kind::Field) {
                out.emplace_back(allocator.create<SkrOffsetStore>(left.getBase(), left.getOffset(), right));
                return SkrExpRes::val(right);
            }
            else {
                out.emplace_back(allocator.create<SkrCopy>(left.get()->toSkrVar(), right));
                return SkrExpRes::val(right);
            }
        }
        else if (kind == AstExp::Kind::FunCall) {
            auto* res = emitFunCall((AstFunCall*) exp);
            return SkrExpRes::val(res);
        }
        else if (kind == AstExp::Kind::Cast) {
            auto* it = (AstCast*) exp;
            auto targetType = exp->type;
            SkrValue* srcVal = emitAndConvert(it->getExp());
            SkrVar* dstVar = createVar("cast", targetType);

            if (getTypeKind(srcVal) == SymbolType::Kind::Integer && targetType->kind == SymbolType::Kind::Float) {
                out.emplace_back(allocator.create<SkrInt2Float>(dstVar, srcVal));
            }
            else if (getTypeKind(srcVal) == SymbolType::Kind::Float && targetType->kind == SymbolType::Kind::Integer) {
                out.emplace_back(allocator.create<SkrFloat2Int>(dstVar, srcVal));
            }
            else {
                sparkError("SkrEmitter", "Failed to cast expression");
            }
            return SkrExpRes::val(dstVar);
        }
        else if (kind == AstExp::Kind::Dot) {
            auto* it = (AstDot*) exp;

            StringRef field = getFieldId(it->getField());
            StringRef tag = getStructTag(it->getFrom()->type);
            int offset = typeTable.getField(tag, field).offset;

            auto inner = emit(it->getFrom());
            return SkrExpRes::field(inner.get(), inner.getOffset() + offset);
        }
        else {
            sparkError("SkrEmitter", "Unknown AstExp: %d", kind);
            return SkrExpRes::val(nullptr);
        }
    }

    StringRef getFieldId(AstExp* exp) {
        if (exp->kind == AstExp::Kind::Var) {
            return ((AstVar*) exp)->getId();
        }
        sparkError("SkrEmitter", "Field is not a var: %d", exp->kind);
        return StringRef::nullInstance();
    }

    SkrValue* emitBinary(AstBinaryExp* exp) {
        auto astOp = exp->getOperator();
        if (astOp == AstBinaryExp::Operator::And) {
            auto falseLabel = labelGen.uniqueInternal("and_false");
            auto endLabel = labelGen.uniqueInternal("and_end");

            SkrVar* result = createVar("and", SymbolIntType::getInstance());
            emitInvertBranch(exp->getLeft(), falseLabel);
            emitInvertBranch(exp->getRight(), falseLabel);
            // true
            out.emplace_back(allocator.create<SkrCopy>(result, getSkrConst(1)));
            out.emplace_back(allocator.create<SkrJump>(endLabel));

            // false
            out.emplace_back(allocator.create<SkrLabel>(falseLabel));
            out.emplace_back(allocator.create<SkrCopy>(result, getSkrConst(0)));

            out.emplace_back(allocator.create<SkrLabel>(endLabel));
            return result;
        }
        else if (astOp == AstBinaryExp::Operator::Or) {
            auto trueLabel = labelGen.uniqueInternal("or_true");
            auto endLabel = labelGen.uniqueInternal("or_end");

            SkrVar* result = createVar("or", SymbolIntType::getInstance());
            emitBranch(exp->getLeft(), trueLabel);
            emitBranch(exp->getRight(), trueLabel);
            // false
            out.emplace_back(allocator.create<SkrCopy>(result, getSkrConst(0)));
            out.emplace_back(allocator.create<SkrJump>(endLabel));

            // true
            out.emplace_back(allocator.create<SkrLabel>(trueLabel));
            out.emplace_back(allocator.create<SkrCopy>(result, getSkrConst(1)));

            out.emplace_back(allocator.create<SkrLabel>(endLabel));
            return result;
        }
        else {
            SkrValue* left = emitAndConvert(exp->getLeft());
            auto op = binaryOpOf(exp->getOperator());
            SkrValue* right = emitAndConvert(exp->getRight());
            SkrVar* dst = createVar(funName, getType(left));
            out.emplace_back(allocator.create<SkrBinary>(dst, left, op, right));
            return dst;
        }
    }

    SkrValue* emitFunCall(AstFunCall* call) {
        auto astArgs = call->getArgs();
        auto skrArgs = BoundArray<SkrValue*>::create(astArgs.size(), allocator);
        for (size_t i = 0; i < astArgs.size(); i++) {
            skrArgs[i] = emitAndConvert(astArgs[i]);
        }
        auto* result = allocator.create<SkrVar>(idGen.unique(call->getFunName(), "r"));
        auto* skrCall = allocator.create<SkrFunCall>(call->getFunName(), skrArgs, result);
        out.emplace_back(skrCall);
        return result;
    }

    SkrValue* emitAndConvert(AstExp* exp) {
        auto res = emit(exp);
        if (res.kind == SkrExpRes::Kind::Val) {
            return res.get();
        }
        else if (res.kind == SkrExpRes::Kind::Ptr) {
            SkrValue* tmpVar = createVar("deref", exp->type);
            out.emplace_back(allocator.create<SkrLoad>(tmpVar, res.get()));
            return tmpVar;
        }
        else if (res.kind == SkrExpRes::Kind::Field) {
            SkrValue* tmpVar = createVar("field", exp->type);
            out.emplace_back(allocator.create<SkrOffsetLoad>(tmpVar, res.getBase(), res.getOffset()));
            return tmpVar;
        }

        sparkError("SkrEmitter", "Unknown SkrExpRes kind: %d", res.kind);
        return nullptr;
    }

    SymbolType::Kind getTypeKind(SkrValue* value) {
        return getType(value)->kind;
    }

    SymbolType* getType(SkrValue* value) {
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

    SymbolType* dereferenceType(SymbolType* t) {
        if (t->kind == SymbolType::Kind::Pointer) {
            auto* it = (SymbolPointerType*) t;
            return it->getVarType();
        }
        sparkError("SkrEmitter", "Failed to dereference type");
        return t;
    }

    SymbolType* dereferenceType(SkrValue* value) {
        return dereferenceType(getType(value));
    }

    StringRef getStructTag(SymbolType* type) {
        if (type->kind == SymbolType::Kind::Structure) {
            return ((SymbolStructureType*) type)->getTag();
        }
        else if (type->kind == SymbolType::Kind::Pointer) {
            return getStructTag(((SymbolPointerType*) type)->getVarType());
        }
        
        sparkError("SkrEmitter", "Expected a structure, but found kind %d", type->kind);
        return StringRef::nullInstance();
    }

    SkrBinary::Operator binaryOpOf(AstBinaryExp::Operator astOp) {
        switch (astOp)
        {
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
            sparkError("SkrBinary", "Can't map AstBinaryExp::Operator to SkrBinary::Operator: %d", astOp);
            return SkrBinary::Operator::Plus;
        }
    }

    SkrBranch::Operator branchOpOf(AstBinaryExp::Operator astOp) {
        switch (astOp)
        {
        case AstBinaryExp::Operator::Equals: return SkrBranch::Operator::Equals;
        case AstBinaryExp::Operator::NotEquals: return SkrBranch::Operator::NotEquals;
        case AstBinaryExp::Operator::LessThan: return SkrBranch::Operator::LessThan;
        case AstBinaryExp::Operator::LessOrEqual: return SkrBranch::Operator::LessOrEqual;
        case AstBinaryExp::Operator::GreaterThan: return SkrBranch::Operator::GreaterThan;
        case AstBinaryExp::Operator::GreaterOrEqual: return SkrBranch::Operator::GreaterOrEqual;
        default:
            sparkError("SkrBinary", "Can't map AstBinaryExp::Operator to SkrBranch::Operator: %d", astOp);
            return SkrBranch::Operator::Equals;
        }
    }

    SkrBranch::Operator invertedBranchOpOf(AstBinaryExp::Operator astOp) {
        switch (astOp)
        {
        case AstBinaryExp::Operator::Equals: return SkrBranch::Operator::NotEquals;
        case AstBinaryExp::Operator::NotEquals: return SkrBranch::Operator::Equals;
        case AstBinaryExp::Operator::LessThan: return SkrBranch::Operator::GreaterOrEqual;
        case AstBinaryExp::Operator::LessOrEqual: return SkrBranch::Operator::GreaterThan;
        case AstBinaryExp::Operator::GreaterThan: return SkrBranch::Operator::LessOrEqual;
        case AstBinaryExp::Operator::GreaterOrEqual: return SkrBranch::Operator::LessThan;
        default:
            sparkError("SkrBinary", "Can't map inv AstBinaryExp::Operator to SkrBranch::Operator: %d", astOp);
            return SkrBranch::Operator::Equals;
        }
    }

    bool isLogicalBin(AstExp* exp) {
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

    void removeUselessJumpToRet() {
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
                } else {
                    break;
                }
            } 
            else {
                break;
            }
        }
    }
    
    SkrVar* createVar(StringRef name, SymbolType* type) {
        auto id = idGen.unique(name);
        symbolTable.declare(id, type);
        return allocator.create<SkrVar>(id);
    }

    SkrVar* createVar(const char* name, SymbolType* type) {
        return createVar(StringRef::cstr(name), type);
    }

    SkrVar* createVar(StringRef name, const char* suffix, SymbolType* type) {
        auto id = idGen.unique(name, suffix);
        symbolTable.declare(id, type);
        return allocator.create<SkrVar>(id);
    }

    SkrConst* getSkrZero() {
        if (skrZero == nullptr) {
            skrZero = allocator.create<SkrConst>(IntConstant::get0());
        }
        return skrZero;
    }

    SkrConst* getSkrOne() {
        if (skrOne == nullptr) {
            skrOne = allocator.create<SkrConst>(IntConstant::get1());
        }
        return skrOne;
    }

    SkrConst* getSkrConst(Constant* c) {
        if (c->isInt()) {
            return getSkrConst(((IntConstant*) c)->val);
        }
        else {
            return allocator.create<SkrConst>(c);
        }
    }

    SkrConst* getSkrConst(int32_t v) {
        if (v == 0) return getSkrZero();
        if (v == 1) return getSkrOne();
        return allocator.create<SkrConst>(allocator.create<IntConstant>(v));
    }

    Allocator& allocator;
    SymbolTable& symbolTable;
    TypeTable& typeTable;
    IdentifierGen& idGen;
    LabelGen& labelGen;
    std::vector<SkrInstruction*>& out;
    SkrVar* funcRetVal = nullptr;
    StringRef funName = StringRef::nullInstance();
    StringRef retLabel = StringRef::nullInstance();
    SkrConst* skrZero = nullptr;
    SkrConst* skrOne = nullptr;
};
