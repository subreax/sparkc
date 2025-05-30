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
    static SkrFunction* emit(AstFunction* func, Allocator& allocator, IdentifierGen& idGen, LabelGen& labelGen, std::vector<SkrInstruction*>& buf) {
        return SkrEmitter(allocator, idGen, labelGen, buf).emit(func);
    }

private:
    SkrEmitter(Allocator& allocator, IdentifierGen& idGen, LabelGen& labelGen, std::vector<SkrInstruction*>& out) 
        : allocator(allocator)
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

        const char* funcResultId = idGen.unique("retval");
        funcResult = allocator.create<SkrVar>(funcResultId);
        retLabel = labelGen.uniqueInternal("ret");
        for (auto* item : func->getBlockItems()) {
            emit(item);
        }
        removeUselessJumpToRet();
        out.emplace_back(allocator.create<SkrLabel>(retLabel));

        auto baInstructions = BoundArray<SkrInstruction*>::fromVector(out, allocator);
        return allocator.create<SkrFunction>(func->getName(), skrParams, baInstructions, funcResultId);
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
            out.emplace_back(allocator.create<SkrCopy>(funcResult, retVal));
            out.emplace_back(allocator.create<SkrJump>(retLabel));
        }
        else if (kind == AstStatement::Kind::Expression) {
            auto* it = (AstExpressionStatement*) st;
            emit(it->getExpression());
        }
        else if (kind == AstStatement::Kind::If) {
            auto* it = (AstIfStatement*) st;
            auto* ifFalseLabel = labelGen.uniqueInternal("false");
            emitBranch(it->getCondition(), ifFalseLabel);
            emit(it->getTrueBranch());
            out.emplace_back(allocator.create<SkrLabel>(ifFalseLabel));
            
            auto* falseBranch = it->getFalseBranch();
            if (falseBranch != nullptr) {
                emit(falseBranch);
            }
        }
        else {
            sparkError("SkrEmitter", "Unknown AstStatement: %d", kind);
        }
    }

    void emitBranch(AstExp* exp, const char* ifFalseLabel) {
        if (isLogicalBin(exp)) {
            AstBinaryExp* binExp = (AstBinaryExp*) exp;
            auto* left = emit(binExp->getLeft());
            auto invertedOp = invertedBranchOpOf(binExp->getOperator());
            auto* right = emit(binExp->getRight());
            auto* branch = allocator.create<SkrBranch>(left, invertedOp, right, ifFalseLabel);
            out.emplace_back(branch);
        } else {
            auto* res = emit(exp);
            auto* branch = allocator.create<SkrBranch>(res, SkrBranch::Operator::Equals, getSkrConst(0), ifFalseLabel);
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
            SkrValue* right = emit(ass->getExp());
            out.emplace_back(allocator.create<SkrCopy>(left, right));
            return left;
        }
        else if (kind == AstExp::Kind::FunCall) {
            return emitFunCall((AstFunCall*) exp);
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

            SkrVar* result = allocator.create<SkrVar>(idGen.unique("and"));
            SkrValue* left = emit(exp->getLeft());
            out.emplace_back(allocator.create<SkrBranch>(left, SkrBranch::Operator::Equals, getSkrConst(0), falseLabel));
            SkrValue* right = emit(exp->getRight());
            out.emplace_back(allocator.create<SkrBranch>(right, SkrBranch::Operator::Equals, getSkrConst(0), falseLabel));
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

            SkrVar* result = allocator.create<SkrVar>(idGen.unique("or"));
            SkrValue* left = emit(exp->getLeft());
            out.emplace_back(allocator.create<SkrBranch>(left, SkrBranch::Operator::NotEquals, getSkrConst(0), trueLabel));
            SkrValue* right = emit(exp->getRight());
            out.emplace_back(allocator.create<SkrBranch>(right, SkrBranch::Operator::NotEquals, getSkrConst(0), trueLabel));
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
            SkrVar* dst = allocator.create<SkrVar>(idGen.unique(funName));
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

    SkrConst* getSkrZero() {
        if (skrZero == nullptr) {
            skrZero = allocator.create<SkrConst>(0);
        }
        return skrZero;
    }

    SkrConst* getSkrOne() {
        if (skrOne == nullptr) {
            skrOne = allocator.create<SkrConst>(1);
        }
        return skrOne;
    }

    SkrConst* getSkrConst(int32_t v) {
        if (v == 0) return getSkrZero();
        if (v == 1) return getSkrOne();
        return allocator.create<SkrConst>(v);
    }

    Allocator& allocator;
    IdentifierGen& idGen;
    LabelGen& labelGen;
    std::vector<SkrInstruction*>& out;
    SkrVar* funcResult = nullptr;
    const char* funName = nullptr;
    const char* retLabel = nullptr;
    SkrConst* skrZero = nullptr;
    SkrConst* skrOne = nullptr;
};
