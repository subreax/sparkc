#pragma once
#include <vector>
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
        const char* funcResultId = idGen.unique("result");
        funcResult = allocator.create<SkrVar>(funcResultId);
        retLabel = labelGen.uniqueInternal("return");
        for (auto* item : func->getBlockItems()) {
            emit(func->getName(), item);
        }
        removeUselessJumpToRet();
        out.emplace_back(allocator.create<SkrLabel>(retLabel));
        return allocator.create<SkrFunction>(func->getName(), funcResultId, out);
    }

private:
    void emit(const char* funName, AstBlockItem* blockItem) {
        auto type = blockItem->getType();
        if (type == AstBlockItem::Type::Declaration) {
            emit(funName, ((AstDeclBlockItem*) blockItem)->getDeclaration());
        }
        else if (type == AstBlockItem::Type::Statement) {
            emit(funName, ((AstStatementBlockItem*) blockItem)->getStatement());
        }
        else {
            printf("[SkrEmitter] Unknown AstBlockItem type: %d", blockItem->getType());
            std::abort();
        }
    }

    void emit(const char* funName, AstDeclaration* decl) {
        if (decl->getType() == AstDeclaration::Type::Var) {
            auto* it = (AstVarDeclaration*) decl;
            auto* initializer = it->getInitializer();
            if (initializer != nullptr) {
                auto* initRes = emit(funName, initializer);
                auto* skrVar = allocator.create<SkrVar>(it->getName());
                out.emplace_back(allocator.create<SkrCopy>(skrVar, initRes));
            }
        }
        else {
            printf("[SkrEmitter] Unknown AstDeclaration type: %d", decl->getType());
            std::abort();
        }
    }

    void emit(const char* funName, AstStatement* st) {
        auto type = st->getType();
        if (type == AstStatement::Type::Return) {
            auto* it = (AstReturnStatement*) st;
            auto* retVal = emit(funName, it->getExpression());
            out.emplace_back(allocator.create<SkrCopy>(funcResult, retVal));
            out.emplace_back(allocator.create<SkrJump>(retLabel));
        }
        else if (type == AstStatement::Type::Expression) {
            auto* it = (AstExpressionStatement*) st;
            emit(funName, it->getExpression());
        }
        else {
            printf("[SkrEmitter] Unknown statement: %d", type);
            std::abort();
        }
    }

    SkrValue* emit(const char* funName, AstExp* exp) {
        auto type = exp->getType();
        if (type == AstExp::EXP_CONSTANT) {
            return allocator.create<SkrConst>(((AstConstantExp*) exp)->getValue());
        }
        else if (type == AstExp::EXP_BINARY) {
            return emitBinary(funName, (AstBinaryExp*) exp);
        }
        else if (type == AstExp::EXP_VAR) {
            return allocator.create<SkrVar>(((AstVar*) exp)->getIdentifier());
        }
        else if (type == AstExp::EXP_ASSIGNMENT) {
            auto* ass = (AstAssignment*) exp;
            SkrValue* left = emit(funName, ass->getVar());
            SkrValue* right = emit(funName, ass->getExp());
            out.emplace_back(allocator.create<SkrCopy>(left, right));
            return left;
        }
        else {
            printf("[SkrEmitter] Unknown AstExp: %d", type);
            std::abort();
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
            out.emplace_back(allocator.create<SkrBranch>(left, SkrBranch::Operator::NotEquals, getSkrConst(1), trueLabel));
            SkrValue* right = emit(funName, exp->getRight());
            out.emplace_back(allocator.create<SkrBranch>(right, SkrBranch::Operator::NotEquals, getSkrConst(1), trueLabel));
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

    SkrBinary::Operator binaryOpOf(AstBinaryExp::Operator astOp) {
        switch (astOp)
        {
        case AstBinaryExp::Operator::Plus: return SkrBinary::Operator::Plus;
        case AstBinaryExp::Operator::Minus: return SkrBinary::Operator::Minus;
        case AstBinaryExp::Operator::Mul: return SkrBinary::Operator::Mul;
        case AstBinaryExp::Operator::Div: return SkrBinary::Operator::Div;
        case AstBinaryExp::Operator::Rem: return SkrBinary::Operator::Rem;
        default:
            printf("Can't map AstBinaryExp::Operator to SkrBinary::Operator");
            std::abort();
            return SkrBinary::Operator::Plus;
        }
    }

    void removeUselessJumpToRet() {
        int i = out.size() - 1;
        for (; i >= 0; i--) {
            auto* skr = out[i];
            auto type = skr->getType();
            if (type == SkrInstruction::Type::Label) {
                continue;
            }

            if (type == SkrInstruction::Type::Jump) {
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
