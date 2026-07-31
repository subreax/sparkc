#pragma once
#include <vector>
#include "sparkc/common/alloc/Allocator.h"
#include "everything.h"

class AstFactory {
public:
    AstFactory(Allocator& allocator)
        : allocator(allocator) { }

    AstProgram* program(const std::vector<AstProgItem*>& items) {
        auto itemsBA = BoundArray<AstProgItem*>::fromVector(items, allocator);
        return allocator.create<AstProgram>(itemsBA);
    }

    // AstProgItems

    AstFunction* function(StringRef name, SymbolType* returnType, const std::vector<AstFunParam*>& params, AstBlock* body) {
        auto paramsBA = BoundArray<AstFunParam*>::fromVector(params, allocator);
        return allocator.create<AstFunction>(name, returnType, paramsBA, body);
    }

    AstFunction* function(const char* name, SymbolType* type, const std::vector<AstFunParam*>& params, AstBlock* body) {
        return function(StringRef::cstr(name), type, params, body);
    }

    AstFunParam* functionParam(StringRef id, SymbolType* type) {
        return allocator.create<AstFunParam>(id, type);
    }

    AstFunParam* functionParam(const char* id, SymbolType* type) {
        return functionParam(StringRef::cstr(id), type);
    }

    AstStruct* struct_(StringRef tag, const std::vector<AstStructField*>& fields) {
        auto fieldsBA = BoundArray<AstStructField*>::fromVector(fields, allocator);
        return allocator.create<AstStruct>(tag, fieldsBA);
    }

    AstStructField* structField(StringRef name, SymbolType* type) {
        return allocator.create<AstStructField>(name, type);
    }

    // AstBlock

    AstBlock* block(const std::vector<AstBlockItem*>& items) {
        auto itemsBA = BoundArray<AstBlockItem*>::fromVector(items, allocator);
        return allocator.create<AstBlock>(itemsBA);
    }

    // AstBlockItems

    AstDeclBlockItem* declBlockItem(AstDeclaration* decl) {
        return allocator.create<AstDeclBlockItem>(decl);
    }

    AstStatementBlockItem* statementBlockItem(AstStatement* statement) {
        return allocator.create<AstStatementBlockItem>(statement);
    }

    // AstDeclaration

    AstVarDeclaration* varDeclaration(StringRef id, SymbolType* type, AstExp* init = nullptr) {
        return allocator.create<AstVarDeclaration>(id, type, init);
    }

    // AstStatement

    AstReturnStatement* returnStatement(AstExp* exp) {
        return allocator.create<AstReturnStatement>(exp);
    }

    AstExpressionStatement* expressionStatement(AstExp* exp) {
        return allocator.create<AstExpressionStatement>(exp);
    }

    AstIfStatement* ifStatement(AstExp* condition, AstStatement* ifTrue, AstStatement* ifFalse = nullptr) {
        return allocator.create<AstIfStatement>(condition, ifTrue, ifFalse);
    }

    AstWhileStatement* whileStatement(AstExp* condition, AstStatement* body) {
        return allocator.create<AstWhileStatement>(condition, body);
    }

    AstCompoundStatement* compoundStatement(AstBlock* block) {
        return allocator.create<AstCompoundStatement>(block);
    }

    // AstExp

    AstAssignment* assignment(AstExp* var, AstExp* exp) {
        return allocator.create<AstAssignment>(var, exp);
    }

    AstDot* dot(AstExp* from, AstExp* field) {
        return allocator.create<AstDot>(from, field);
    }

    AstBinaryExp* binaryExp(AstExp* left, AstBinaryExp::Operator op, AstExp* right) {
        return allocator.create<AstBinaryExp>(left, op, right);
    }

    AstBinaryExp* binaryExp(AstExp* left, TokenKind op, AstExp* right) {
        return binaryExp(left, AstBinaryExp::toBinaryOperator(op), right);
    }

    AstConstantExp* intConstantExp(int32_t value) {
        auto* constant = allocator.create<IntConstant>(value);
        return allocator.create<AstConstantExp>(constant);
    }

    AstConstantExp* floatConstantExp(float value) {
        auto* constant = allocator.create<FloatConstant>(value);
        return allocator.create<AstConstantExp>(constant);
    }

    AstFunCall* funCall(StringRef name, const std::vector<AstExp*>& args) {
        auto argsBA = BoundArray<AstExp*>::fromVector(args, allocator);
        return allocator.create<AstFunCall>(name, argsBA);
    }

    AstVar* var(StringRef id) {
        return allocator.create<AstVar>(id);
    }

    AstStructInit* structInit(StringRef tag, const std::vector<AstExp*>& args) {
        return structInit(tag, BoundArray<AstExp*>::fromVector(args, allocator));
    }

    AstStructInit* structInit(StringRef tag, const BoundArray<AstExp*>& args) {
        return allocator.create<AstStructInit>(tag, args);
    }

    AstCast* cast(AstExp* exp, SymbolType* targetType) {
        return allocator.create<AstCast>(exp, targetType);
    }

    AstDereference* dereference(AstExp* exp, SymbolType* type) {
        return allocator.create<AstDereference>(exp, type);
    }

private:
    Allocator& allocator;
};
