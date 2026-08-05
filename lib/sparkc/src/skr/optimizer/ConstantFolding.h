#pragma once
#include "SkrOptimizerUtils.h"
#include "sparkc/skr/SkrFactory.h"
#include "sparkc/skr/instr/everything.h"
#include <cmath>

class ConstantFolding {
public:
    static void run(SkrFactory& skrf, std::vector<SkrInstruction*>& instructions) {
        ConstantFolding(skrf).run(instructions);
    }

private:
    explicit ConstantFolding(SkrFactory& skrf)
        : skrf(skrf) { }

    void run(std::vector<SkrInstruction*>& instructions) {
        auto it = instructions.begin();
        auto end = instructions.end();
        while (it != end) {
            SkrInstruction* initial = *it;
            SkrInstruction* simplified = initial;
            switch (initial->kind) {
            case SkrInstruction::Kind::Binary:
                simplified = processBinary((SkrBinary*) initial);
                break;

            case SkrInstruction::Kind::Branch:
                simplified = processBranch((SkrBranch*) initial);
                break;

            case SkrInstruction::Kind::Int2Float:
                simplified = processInt2Float((SkrInt2Float*) initial);
                break;

            case SkrInstruction::Kind::Float2Int:
                simplified = processFloat2Int((SkrFloat2Int*) initial);
                break;

            default:
                break;
            }

            if (initial != simplified) {
                *it = simplified;
            }

            it++;
        }

        SkrOptimizerUtils::filterNullptrs(instructions);
    }

    SkrInstruction* processBinary(SkrBinary* it) {
        auto* left = it->getLeft();
        auto* right = it->getRight();
        if (left->isConst() && right->isConst()) {
            return skrf.copy(
                it->getDst(),
                evaluate(left->toSkrConst(), it->getOperator(), right->toSkrConst())
            );
        }
        return it;
    }

    SkrInstruction* processBranch(SkrBranch* it) {
        auto* left = it->getLeft();
        auto* right = it->getRight();
        if (left->isConst() && right->isConst()) {
            return evaluate(it);
        }
        else if (left->isVar() && *left == *right) {
            return it->getOperator() == SkrBranch::Operator::Equals
                ? skrf.jump(it->getLabel())
                : nullptr;
        }
        return it;
    }

    SkrInstruction* processInt2Float(SkrInt2Float* it) {
        if (it->getSrc()->isConst()) {
            auto* constant = it->getSrc()->toSkrConst()->getConst();
            return skrf.copy(
                it->getDst(),
                skrf.constant((float) constant->intValue())
            );
        }
        return it;
    }

    SkrInstruction* processFloat2Int(SkrFloat2Int* it) {
        if (it->getSrc()->isConst()) {
            auto* constant = it->getSrc()->toSkrConst()->getConst();
            return skrf.copy(
                it->getDst(),
                skrf.constant((int32_t) constant->floatValue())
            );
        }
        return it;
    }

    SkrConst* evaluate(SkrConst* left, SkrBinary::Operator op, SkrConst* right) {
        auto* leftC = left->getConst();
        auto* rightC = right->getConst();
        if (leftC->isInt()) {
            return evaluate(leftC->intValue(), op, rightC->intValue());
        }
        else if (leftC->isFloat()) {
            return evaluate(leftC->floatValue(), op, rightC->floatValue());
        }
        sparkError("ConstantFolding", "Unsupported binary operator: %d", op);
        return nullptr;
    }

    SkrConst* evaluate(int32_t left, SkrBinary::Operator op, int32_t right) {
        int32_t res = 0;
        switch (op) {
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
            }
            else {
                // todo: replace with normal exception
                sparkError("SkrOptimizer", "Division by zero");
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

        return skrf.constant(res);
    }

    SkrConst* evaluate(float left, SkrBinary::Operator op, float right) {
        float result = 0;
        switch (op) {
        case SkrBinary::Operator::Plus:
            result = left + right;
            break;

        case SkrBinary::Operator::Minus:
            result = left - right;
            break;

        case SkrBinary::Operator::Mul:
            result = left * right;
            break;

        case SkrBinary::Operator::Div:
            if (right != 0) {
                result = left / right;
            }
            else {
                // todo: replace with normal exception
                sparkError("SkrOptimizer", "Division by zero");
            }
            break;

        case SkrBinary::Operator::Rem:
            result = fmodf(left, right);
            break;

        case SkrBinary::Operator::Equals:
            result = left == right;
            break;

        case SkrBinary::Operator::NotEquals:
            result = left != right;
            break;

        case SkrBinary::Operator::LessThan:
            result = left < right;
            break;

        case SkrBinary::Operator::LessOrEqual:
            result = left <= right;
            break;

        case SkrBinary::Operator::GreaterThan:
            result = left > right;
            break;

        case SkrBinary::Operator::GreaterOrEqual:
            result = left >= right;
            break;

        default:
            sparkError("SkrOptimizer", "Unsupported binary operator: %d", op);
        }

        return skrf.constant(result);
    }

    SkrInstruction* evaluate(SkrBranch* branch) {
        auto* left = branch->getLeft()->toSkrConst()->getConst();
        auto* right = branch->getRight()->toSkrConst()->getConst();

        bool result;
        if (left->isInt()) {
            result = evaluate(left->intValue(), branch->getOperator(), right->intValue());
        }
        else if (left->isFloat()) {
            result = evaluate(left->floatValue(), branch->getOperator(), right->floatValue());
        }
        else {
            sparkError("ConstantFolding", "Unknown type for comparison: %d", left->type->kind);
            return nullptr;
        }

        if (result) {
            return skrf.jump(branch->getLabel());
        }
        else {
            return nullptr;
        }
    }

    bool evaluate(int32_t left, SkrBranch::Operator op, int32_t right) {
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

    bool evaluate(float left, SkrBranch::Operator op, float right) {
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

    SkrFactory& skrf;
};
