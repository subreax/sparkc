#pragma once
#include <vector>
#include "../common/Error.h"
#include "../common/LinearAllocator.h"
#include "../common/IdentifierGen.h"
#include "../common/LabelGen.h"
#include "../frontend/ast/exp/everything.h"
#include "SkrFunction.h"
#include "instr/everything.h"

class SkrEmitter {
public:
    SkrEmitter(LinearAllocator& allocator, IdentifierGen& idGen, LabelGen& labelGen, std::vector<SkrInstruction*>& out) 
        : allocator(allocator)
        , idGen(idGen)
        , labelGen(labelGen)
        , out(out) {  }

    SkrFunction* emit(AstFunction* func) {
        const auto& astParams = func->getParams();
        auto skrParams = BoundArray<SkrVar*>::create(astParams.size(), allocator);
        for (size_t i = 0; i < astParams.size(); i++) {
            auto* astParam = astParams[i];
            auto* skrParam = allocator.create<SkrVar>(astParam->getIdentifier());
            skrParams[i] = skrParam;
        }

        const char* funcResultId = idGen.unique("result");
        funcResult = allocator.create<SkrVar>(funcResultId);
        retLabel = labelGen.uniqueInternal("return");
        for (auto* item : func->getBlockItems()) {
            emit(func->getName(), item);
        }
        removeUselessJumpToRet();
        out.emplace_back(allocator.create<SkrLabel>(retLabel));
        return allocator.create<SkrFunction>(func->getName(), skrParams, out, funcResultId);
    }

private:
    void emit(const char* funName, AstBlockItem* blockItem) {
        auto kind = blockItem->kind;
        if (kind == AstBlockItem::Kind::Declaration) {
            emit(funName, ((AstDeclBlockItem*) blockItem)->getDeclaration());
        }
        else if (kind == AstBlockItem::Kind::Statement) {
            emit(funName, ((AstStatementBlockItem*) blockItem)->getStatement());
        }
        else {
            sparkError("SkrEmitter", "Unknown AstBlockItem: %d", kind);
        }
    }

    void emit(const char* funName, AstDeclaration* decl) {
        if (decl->kind == AstDeclaration::Kind::Var) {
            auto* it = (AstVarDeclaration*) decl;
            auto* initializer = it->getInitializer();
            if (initializer != nullptr) {
                auto* initRes = emit(funName, initializer);
                auto* skrVar = allocator.create<SkrVar>(it->getName());
                out.emplace_back(allocator.create<SkrCopy>(skrVar, initRes));
            }
        }
        else {
            sparkError("SkrEmitter", "Unknown AstDeclaration: %d", decl->kind);
        }
    }

    void emit(const char* funName, AstStatement* st) {
        auto kind = st->kind;
        if (kind == AstStatement::Kind::Return) {
            auto* it = (AstReturnStatement*) st;
            auto* retVal = emit(funName, it->getExpression());
            out.emplace_back(allocator.create<SkrCopy>(funcResult, retVal));
            out.emplace_back(allocator.create<SkrJump>(retLabel));
        }
        else if (kind == AstStatement::Kind::Expression) {
            auto* it = (AstExpressionStatement*) st;
            emit(funName, it->getExpression());
        }
        else {
            sparkError("SkrEmitter", "Unknown AstStatement: %d", kind);
        }
    }

    SkrValue* emit(const char* funName, AstExp* exp) {
        auto kind = exp->kind;
        if (kind == AstExp::Kind::Constant) {
            auto* it = (AstConstantExp*) exp;
            return getSkrConst(it->getValue());
        }
        else if (kind == AstExp::Kind::Binary) {
            return emitBinary(funName, (AstBinaryExp*) exp);
        }
        else if (kind == AstExp::Kind::Var) {
            return allocator.create<SkrVar>(((AstVar*) exp)->getIdentifier());
        }
        else if (kind == AstExp::Kind::Assignment) {
            auto* ass = (AstAssignment*) exp;
            SkrValue* left = emit(funName, ass->getVar());
            SkrValue* right = emit(funName, ass->getExp());
            out.emplace_back(allocator.create<SkrCopy>(left, right));
            return left;
        }
        else if (kind == AstExp::Kind::FunCall) {
            return emitFunCall(funName, (AstFunCall*) exp);
        }
        else {
            sparkError("SkrEmitter", "Unknown AstExp: %d", kind);
            return nullptr;
        }
    }

    SkrValue* emitBinary(const char* funName, AstBinaryExp* exp) {
        auto astOp = exp->getOperator();
        if (astOp == AstBinaryExp::Operator::And) {
            const char* falseLabel = labelGen.uniqueInternal("false");
            const char* endLabel = labelGen.uniqueInternal("end");

            SkrVar* result = allocator.create<SkrVar>(idGen.unique("and"));
            SkrValue* left = emit(funName, exp->getLeft());
            out.emplace_back(allocator.create<SkrBranch>(left, SkrBranch::Operator::Equals, getSkrConst(0), falseLabel));
            SkrValue* right = emit(funName, exp->getRight());
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
            const char* trueLabel = labelGen.uniqueInternal("true");
            const char* endLabel = labelGen.uniqueInternal("end");

            SkrVar* result = allocator.create<SkrVar>(idGen.unique("or"));
            SkrValue* left = emit(funName, exp->getLeft());
            out.emplace_back(allocator.create<SkrBranch>(left, SkrBranch::Operator::NotEquals, getSkrConst(0), trueLabel));
            SkrValue* right = emit(funName, exp->getRight());
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
            SkrValue* left = emit(funName, exp->getLeft());
            auto op = binaryOpOf(exp->getOperator());
            SkrValue* right = emit(funName, exp->getRight());
            SkrVar* dst = allocator.create<SkrVar>(idGen.unique(funName));
            out.emplace_back(allocator.create<SkrBinary>(dst, left, op, right));
            return dst;
        }
    }

    SkrValue* emitFunCall(const char* funName, AstFunCall* call) {
        auto astArgs = call->getArgs();
        auto skrArgs = BoundArray<SkrValue*>::create(astArgs.size(), allocator);
        for (size_t i = 0; i < astArgs.size(); i++) {
            skrArgs[i] = emit(funName, astArgs[i]);
        }
        auto* result = allocator.create<SkrVar>(idGen.unique(call->getFunName(), "res"));
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
        default:
            sparkError("SkrBinary", "Can't map AstBinaryExp::Operator to SkrBinary::Operator: %d", astOp);
            return SkrBinary::Operator::Plus;
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

    LinearAllocator& allocator;
    IdentifierGen& idGen;
    LabelGen& labelGen;
    std::vector<SkrInstruction*>& out;
    SkrVar* funcResult = nullptr;
    const char* retLabel = nullptr;
    SkrConst* skrZero = nullptr;
    SkrConst* skrOne = nullptr;
};
