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
        for (auto* st : func->getStatements()) {
            emit(func->getName(), st);
        }
        removeUselessJumpToRet();
        out.emplace_back(allocator.create<SkrLabel>(retLabel));
        return allocator.create<SkrFunction>(func->getName(), funcResultId, out);
    }

private:
    void emit(const char* funName, AstStatement* st) {
        auto type = st->getType();
        if (type == AstStatement::Type::Return) {
            auto* it = (AstReturnStatement*) st;
            auto* retVal = emit(funName, it->getExpression());
            out.emplace_back(allocator.create<SkrCopy>(funcResult, retVal));
            out.emplace_back(allocator.create<SkrJump>(retLabel));
        }
        else {
            printf("Unknown statement: %d", type);
            std::abort();
        }
    }

    SkrValue* emit(const char* funName, const AstExp* exp) {
        if (exp->getType() == AstExp::EXP_CONSTANT) {
            return allocator.create<SkrConst>(((AstConstantExp*) exp)->getValue());
        }
        else if (exp->getType() == AstExp::EXP_BINARY) {
            return emitBinary(funName, (AstBinaryExp*) exp);
        }
        else {
            printf("Unknown AstExp: %d", exp->getType());
            std::abort();
            return nullptr;
        }
    }

    SkrValue* emitBinary(const char* funName, AstBinaryExp* exp) {
        SkrValue* left = emit(funName, exp->getLeft());
        auto op = binaryOpOf(exp->getOperator());
        SkrValue* right = emit(funName, exp->getRight());
        SkrVar* dst = allocator.create<SkrVar>(idGen.unique(funName));
        out.emplace_back(allocator.create<SkrBinary>(dst, left, op, right));
        return dst;
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

    LinearAllocator& allocator;
    IdentifierGen& idGen;
    LabelGen& labelGen;
    std::vector<SkrInstruction*>& out;
    SkrVar* funcResult = nullptr;
    const char* retLabel = nullptr;
};
