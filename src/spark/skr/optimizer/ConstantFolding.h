#pragma once
#include <cmath>
#include "../../common/alloc/Allocator.h"
#include "../instr/everything.h"
#include "SkrOptimizerUtils.h"

class ConstantFolding {
public:
    static void run(Allocator& a, std::vector<SkrInstruction*>& instructions) {
        auto it = instructions.begin();
        auto end = instructions.end();
        while (it != end) {
            auto* skr = *it;
            if (skr->kind == SkrInstruction::Kind::Binary) {
                auto* bin = (SkrBinary*) skr;
                auto* left = bin->getLeft();
                auto* right = bin->getRight();
                if (left->isConst() && right->isConst()) {
                    *it = a.create<SkrCopy>(
                        bin->getDst(), 
                        evaluate(a, left->toSkrConst(), bin->getOperator(), right->toSkrConst())
                    );
                }
            }
            else if (skr->kind == SkrInstruction::Kind::Branch) {
                auto* br = (SkrBranch*) skr;
                auto* left = br->getLeft();
                auto* right = br->getRight();
                if (left->isConst() && right->isConst()) {
                    *it = evaluateConst(a, br);
                }
                else if (left->isVar() && *left == *right) {
                    if (br->getOperator() == SkrBranch::Operator::Equals) {
                        *it = a.create<SkrJump>(br->getLabel());
                    } else {
                        *it = nullptr;
                    }
                }
            }
            else if (skr->kind == SkrInstruction::Kind::Int2Float) {
                auto* i2f = (SkrInt2Float*) skr;
                if (i2f->getSrc()->isConst()) {
                    auto* c = i2f->getSrc()->toSkrConst()->getConst();
                    float evaluated = (float) c->intValue();
                    *it = a.create<SkrCopy>(
                        i2f->getDst(),
                        a.create<SkrConst>(a.create<FloatConstant>(evaluated))
                    );
                }
            }
            else if (skr->kind == SkrInstruction::Kind::Float2Int) {
                auto* f2i = (SkrInt2Float*) skr;
                if (f2i->getSrc()->isConst()) {
                    auto* c = f2i->getSrc()->toSkrConst()->getConst();
                    int32_t evaluated = (int32_t) c->floatValue();
                    *it = a.create<SkrCopy>(
                        f2i->getDst(),
                        a.create<SkrConst>(a.create<IntConstant>(evaluated))
                    );
                }
            }
            it++;
        }

        SkrOptimizerUtils::filterNullptrs(instructions);
    }

private:
    static SkrConst* evaluate(Allocator& a, SkrConst* left, SkrBinary::Operator op, SkrConst* right) {
        auto* leftC = left->getConst();
        auto* rightC = right->getConst();
        Constant* c = nullptr;
        if (leftC->isInt()) {
            c = evaluate(a, leftC->intValue(), op, rightC->intValue());
        }
        else if (leftC->isFloat()) {
            c = evaluate(a, leftC->floatValue(), op, rightC->floatValue());
        }
        return a.create<SkrConst>(c);
    }

    static IntConstant* evaluate(Allocator& a, int32_t left, SkrBinary::Operator op, int32_t right) {
        int32_t res = 0;
        switch (op)
        {
        case SkrBinary::Operator::Plus:
            res = left + right;
            break;

        case SkrBinary::Operator::Minus:
            res = left - right;
            break;

        case SkrBinary::Operator::Mul:
            res = left * right;
            break;

        case SkrBinary::Operator::Div:
            if (right != 0) {
                res = left / right;
            } else {
                sparkError("SkrOptimizer", "Division by zero"); // todo: replace with normal exception
            }
            break;

        case SkrBinary::Operator::Rem:
            res = left % right;
            break;

        case SkrBinary::Operator::Equals:
            res = left == right;
            break;

        case SkrBinary::Operator::NotEquals:
            res = left != right;
            break;

        case SkrBinary::Operator::LessThan:
            res = left < right;
            break;

        case SkrBinary::Operator::LessOrEqual:
            res = left <= right;
            break;

        case SkrBinary::Operator::GreaterThan:
            res = left > right;
            break;

        case SkrBinary::Operator::GreaterOrEqual:
            res = left >= right;
            break;

        default:
            sparkError("SkrOptimizer", "Unsupported binary operator: %d", op);
        }

        return a.create<IntConstant>(res);
    }



    static FloatConstant* evaluate(Allocator& a, float left, SkrBinary::Operator op, float right) {
        float res = 0;
        switch (op)
        {
        case SkrBinary::Operator::Plus:
            res = left + right;
            break;

        case SkrBinary::Operator::Minus:
            res = left - right;
            break;

        case SkrBinary::Operator::Mul:
            res = left * right;
            break;

        case SkrBinary::Operator::Div:
            if (right != 0) {
                res = left / right;
            } else {
                sparkError("SkrOptimizer", "Division by zero"); // todo: replace with normal exception
            }
            break;

        case SkrBinary::Operator::Rem:
            res = fmodf(left, right);
            break;

        case SkrBinary::Operator::Equals:
            res = left == right;
            break;

        case SkrBinary::Operator::NotEquals:
            res = left != right;
            break;

        case SkrBinary::Operator::LessThan:
            res = left < right;
            break;

        case SkrBinary::Operator::LessOrEqual:
            res = left <= right;
            break;

        case SkrBinary::Operator::GreaterThan:
            res = left > right;
            break;

        case SkrBinary::Operator::GreaterOrEqual:
            res = left >= right;
            break;

        default:
            sparkError("SkrOptimizer", "Unsupported binary operator: %d", op);
        }

        return a.create<FloatConstant>(res);
    }

    static SkrInstruction* evaluateConst(Allocator& a, SkrBranch* branch) {
        auto* left = branch->getLeft()->toSkrConst()->getConst();
        auto* right = branch->getRight()->toSkrConst()->getConst();

        bool res;
        if (left->isInt()) {
            res = evaluate(a, left->intValue(), branch->getOperator(), right->intValue());
        }
        else if (left->isFloat()) {
            res = evaluate(a, left->floatValue(), branch->getOperator(), right->floatValue());
        }
        else {
            sparkError("ConstantFolding", "Unknown type for comparison: %d", left->type->kind);
            return nullptr;
        }

        if (res) {
            return a.create<SkrJump>(branch->getLabel());
        } else {
            return nullptr;
        }
    }

    static bool evaluate(Allocator& a, int32_t left, SkrBranch::Operator op, int32_t right) {
        switch (op) {
        case SkrBranch::Operator::Equals: return left == right;
        case SkrBranch::Operator::NotEquals: return left != right;
        case SkrBranch::Operator::LessThan: return left < right;
        case SkrBranch::Operator::LessOrEqual: return left <= right;
        case SkrBranch::Operator::GreaterThan: return left > right;
        case SkrBranch::Operator::GreaterOrEqual: return left >= right;
        default:
            sparkError("ConstantFolding", "Unknown branch operator: %d", op);
            return false;
        }
    }

    static bool evaluate(Allocator& a, float left, SkrBranch::Operator op, float right) {
        switch (op) {
        case SkrBranch::Operator::Equals: return left == right;
        case SkrBranch::Operator::NotEquals: return left != right;
        case SkrBranch::Operator::LessThan: return left < right;
        case SkrBranch::Operator::LessOrEqual: return left <= right;
        case SkrBranch::Operator::GreaterThan: return left > right;
        case SkrBranch::Operator::GreaterOrEqual: return left >= right;
        default:
            sparkError("ConstantFolding", "Unknown branch operator: %d", op);
            return false;
        }
    }
};