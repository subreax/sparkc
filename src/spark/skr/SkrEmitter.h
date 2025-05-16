#pragma once
#include <vector>
#include "../common/LinearAllocator.h"
#include "../common/IdentifierGen.h"
#include "../frontend/ast/exp/everything.h"
#include "instr/SkrBinary.h"

class SkrEmitter {
public:
    SkrEmitter(LinearAllocator& allocator, IdentifierGen& idGen) 
        : allocator(allocator)
        , idGen(idGen) {  }

    SkrValue* emit(const char* funName, AstExp* exp) {
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

    std::vector<SkrInstruction*>& getSkrs() { return out; }

private:
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

    LinearAllocator& allocator;
    IdentifierGen& idGen;
    std::vector<SkrInstruction*> out;
};
