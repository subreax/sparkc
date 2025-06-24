#pragma once
#include <vector>
#include "../common/Error.h"
#include "../common/alloc/LinearAllocator.h"
#include "../common/IdentifierGen.h"
#include "../common/LabelGen.h"
#include "../frontend/ast/exp/everything.h"
#include "SkrFunction.h"
#include "instr/everything.h"

class SkrEmitter {
public:
    static SkrFunction* emit(AstFunction* func, Allocator& allocator, SymbolTable& table, IdentifierGen& idGen, LabelGen& labelGen, std::vector<SkrInstruction*>& buf) {
        return SkrEmitter(allocator, idGen, labelGen, table, buf).emit(func);
    }

private:
    SkrEmitter(Allocator& allocator, IdentifierGen& idGen, LabelGen& labelGen, SymbolTable& table, std::vector<SkrInstruction*>& out) 
        : allocator(allocator)
        , table(table)
        , idGen(idGen)
        , labelGen(labelGen)
        , out(out) {  }

    SkrFunction* emit(AstFunction* func) {
        funName = func->getName();

        const auto& astParams = func->getParams();
        auto skrParams = BoundArray<SkrVar*>::create(astParams.size(), allocator);
        for (size_t i = 0; i < astParams.size(); i++) {
            auto* astParam = astParams[i];
            auto* skrParam = allocator.create<SkrVar>(astParam->getIdentifier());
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
            if (initializer != nullptr) {
                auto* initRes = emit(initializer);
                auto* skrVar = allocator.create<SkrVar>(it->getName());
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
            auto* retVal = emit(it->getExpression());
            out.emplace_back(allocator.create<SkrCopy>(funcRetVal, retVal));
            out.emplace_back(allocator.create<SkrJump>(retLabel));
        }
        else if (kind == AstStatement::Kind::Expression) {
            auto* it = (AstExpressionStatement*) st;
            emit(it->getExpression());
        }
        else if (kind == AstStatement::Kind::If) {
            auto* it = (AstIfStatement*) st;
            auto* ifFalseLabel = labelGen.uniqueInternal("false");
            emitInvertBranch(it->getCondition(), ifFalseLabel);
            emit(it->getTrueBranch());

            auto* falseBranch = it->getFalseBranch();
            if (falseBranch != nullptr) {
                auto* endLabel = labelGen.uniqueInternal("end");
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
            auto* startLabel = labelGen.uniqueInternal("start");
            auto* endLabel = labelGen.uniqueInternal("end");
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

    void emitBranch(AstExp* exp, const char* trueLabel) {
        emitBranch(exp, trueLabel, false);
    }

    void emitInvertBranch(AstExp* exp, const char* falseLabel) {
        emitBranch(exp, falseLabel, true);
    }

    void emitBranch(AstExp* exp, const char* label, bool invert) {
        if (isLogicalBin(exp) && exp->type->kind == SymbolType::Kind::Integer) {
            AstBinaryExp* binExp = (AstBinaryExp*) exp;
            auto* left = emit(binExp->getLeft());
            SkrBranch::Operator skrOp;
            if (invert) {
                skrOp = invertedBranchOpOf(binExp->getOperator());
            } else {
                skrOp = branchOpOf(binExp->getOperator());
            }
            auto* right = emit(binExp->getRight());
            auto* branch = allocator.create<SkrBranch>(left, skrOp, right, label);
            out.emplace_back(branch);
        } else {
            auto* res = emit(exp);
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

    SkrValue* emit(AstExp* exp) {
        auto kind = exp->kind;
        if (kind == AstExp::Kind::Constant) {
            auto* it = (AstConstantExp*) exp;
            return getSkrConst(it->getValue());
        }
        else if (kind == AstExp::Kind::Binary) {
            return emitBinary((AstBinaryExp*) exp);
        }
        else if (kind == AstExp::Kind::Var) {
            return allocator.create<SkrVar>(((AstVar*) exp)->getIdentifier());
        }
        else if (kind == AstExp::Kind::Assignment) {
            auto* ass = (AstAssignment*) exp;
            SkrValue* left = emit(ass->getVar());
            if (!left->isVar()) {
                sparkError("SkrEmitter", "Left is expected to be a variable");
            }
            SkrValue* right = emit(ass->getExp());
            out.emplace_back(allocator.create<SkrCopy>(left->toSkrVar(), right));
            return left;
        }
        else if (kind == AstExp::Kind::FunCall) {
            return emitFunCall((AstFunCall*) exp);
        }
        else if (kind == AstExp::Kind::Cast) {
            auto* it = (AstCast*) exp;
            auto targetType = exp->type;
            SkrValue* srcVal = emit(it->getExp());
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
            return dstVar;
        }
        else {
            sparkError("SkrEmitter", "Unknown AstExp: %d", kind);
            return nullptr;
        }
    }

    SkrValue* emitBinary(AstBinaryExp* exp) {
        auto astOp = exp->getOperator();
        if (astOp == AstBinaryExp::Operator::And) {
            const char* falseLabel = labelGen.uniqueInternal("and_false");
            const char* endLabel = labelGen.uniqueInternal("and_end");

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
            const char* trueLabel = labelGen.uniqueInternal("or_true");
            const char* endLabel = labelGen.uniqueInternal("or_end");

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
            SkrValue* left = emit(exp->getLeft());
            auto op = binaryOpOf(exp->getOperator());
            SkrValue* right = emit(exp->getRight());
            SkrVar* dst = createVar(funName, getType(left));
            out.emplace_back(allocator.create<SkrBinary>(dst, left, op, right));
            return dst;
        }
    }

    SkrValue* emitFunCall(AstFunCall* call) {
        auto astArgs = call->getArgs();
        auto skrArgs = BoundArray<SkrValue*>::create(astArgs.size(), allocator);
        for (size_t i = 0; i < astArgs.size(); i++) {
            skrArgs[i] = emit(astArgs[i]);
        }
        auto* result = allocator.create<SkrVar>(idGen.unique(call->getFunName(), "v"));
        auto* skrCall = allocator.create<SkrFunCall>(call->getFunName(), skrArgs, result);
        out.emplace_back(skrCall);
        return result;
    }

    SymbolType::Kind getTypeKind(SkrValue* value) {
        return getType(value)->kind;
    }

    SymbolType* getType(SkrValue* value) {
        if (value->isConst()) {
            return value->toSkrConst()->getConst()->type;
        }
        else if (value->isVar()) {
            return table.get(value->toSkrVar()->getId());
        }
        else {
            sparkError("SkrEmitter", "Unknown SkrValue kind");
            return nullptr;
        }
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
    
    SkrVar* createVar(const char* name, SymbolType* type) {
        auto* id = idGen.unique(name);
        table.declare(id, type);
        return allocator.create<SkrVar>(id);
    }

    SkrVar* createVar(const char* name, const char* suffix, SymbolType* type) {
        auto* id = idGen.unique(name, suffix);
        table.declare(id, type);
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
    SymbolTable& table;
    IdentifierGen& idGen;
    LabelGen& labelGen;
    std::vector<SkrInstruction*>& out;
    SkrVar* funcRetVal = nullptr;
    const char* funName = nullptr;
    const char* retLabel = nullptr;
    SkrConst* skrZero = nullptr;
    SkrConst* skrOne = nullptr;
};
