#pragma once
#include <vector>
#include "sparkc/common/alloc/Allocator.h"
#include "sparkc/skr/SkrFunction.h"
#include "sparkc/skr/instr/everything.h"

class SkrFactory {
public:
    SkrFactory(Allocator& allocator)
        : allocator(allocator) { }

    SkrFunction* function(
        StringRef name,
        const std::vector<SkrVar*>& params,
        const std::vector<SkrInstruction*>& instructions,
        const SkrVar* retVar
    ) {
        return allocator.create<SkrFunction>(
            name,
            BoundArray<SkrVar*>::fromVector(params, allocator),
            BoundArray<SkrInstruction*>::fromVector(instructions, allocator),
            retVar
        );
    }

    SkrVar* var(StringRef id) {
        return allocator.create<SkrVar>(id);
    }

    SkrLabel* label(StringRef label) {
        return allocator.create<SkrLabel>(label);
    }

    SkrJump* jump(StringRef label) {
        return allocator.create<SkrJump>(label);
    }

    SkrCopy* copy(SkrVar* to, SkrValue* from) {
        return allocator.create<SkrCopy>(to, from);
    }

    SkrBranch* branch(SkrValue* left, SkrBranch::Operator op, SkrValue* right, StringRef label) {
        return allocator.create<SkrBranch>(left, op, right, label);
    }

    SkrGetAddr* getAddr(SkrVar* dst, SkrVar* src) {
        return allocator.create<SkrGetAddr>(dst, src);
    }

    SkrInt2Float* int2Float(SkrVar* dst, SkrValue* src) {
        return allocator.create<SkrInt2Float>(dst, src);
    }

    SkrFloat2Int* float2Int(SkrVar* dst, SkrValue* src) {
        return allocator.create<SkrFloat2Int>(dst, src);
    }

    SkrBinary* binary(SkrVar* dst, SkrValue* left, SkrBinary::Operator op, SkrValue* right) {
        return allocator.create<SkrBinary>(dst, left, op, right);
    }

    SkrFunCall* funCall(StringRef name, const std::vector<SkrValue*>& args, const SkrVar* retVar) {
        return allocator.create<SkrFunCall>(
            name,
            BoundArray<SkrValue*>::fromVector(args, allocator),
            retVar
        );
    }

    SkrCopyToOffset* copyToOffset(SkrVar* to, int offset, SkrValue* from) {
        return allocator.create<SkrCopyToOffset>(to, offset, from);
    }

    SkrCopyFromOffset* copyFromOffset(SkrVar* to, SkrVar* from, int offset) {
        return allocator.create<SkrCopyFromOffset>(to, from, offset);
    }

    SkrConst* constant(int32_t value) {
        if (value == 0) {
            return SkrConst::getInt0();
        }
        if (value == 1) {

            return SkrConst::getInt1();
        }
        return allocator.create<SkrConst>(allocator.create<IntConstant>(value));
    }

    SkrConst* constant(float value) {
        return allocator.create<SkrConst>(allocator.create<FloatConstant>(value));
    }

    SkrConst* constant(Constant* c) {
        switch (c->type->kind) {
        case SymbolType::Kind::Integer:
            return constant(c->intValue());
        case SymbolType::Kind::Float:
            return constant(c->floatValue());
        default:
            sparkError("SkrFactory", "Unknown Constant kind: %d", c->type->kind);
            return nullptr;
        }
    }

private:
    Allocator& allocator;
};
