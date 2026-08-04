#pragma once
#include "sparkc/common/Error.h"
#include "sparkc/common/IdentifierGen.h"
#include "sparkc/common/LabelGen.h"
#include "sparkc/frontend/ast/everything.h"
#include "sparkc/skr/SkrFunction.h"
#include "sparkc/skr/SkrFactory.h"
#include "sparkc/skr/instr/everything.h"
#include "sparkc/symbol/SymbolTable.h"
#include "sparkc/type/TypeTable.h"
#include "value/SkrExpRes.h"
#include <vector>

class SkrEmitter {
public:
    static SkrFunction* emit(
        AstFunction* func,
        SkrFactory& factory,
        SymbolTable& symbolTable,
        TypeTable& typeTable,
        IdentifierGen& idGen,
        LabelGen& labelGen,
        std::vector<SkrInstruction*>& buf
    );

private:
    SkrEmitter(
        SkrFactory& factory,
        IdentifierGen& idGen,
        LabelGen& labelGen,
        SymbolTable& symbolTable,
        TypeTable& typeTable,
        std::vector<SkrInstruction*>& out
    );

    SkrFunction* emit(AstFunction* func);

    void emit(const AstBlock* block);
    void emit(AstBlockItem* blockItem);
    void emit(AstDeclaration* decl);
    void emit(AstStatement* st);
    void emit(AstVarDeclaration* decl);
    void emit(AstReturnStatement* st);
    void emit(AstExpressionStatement* st);
    void emit(AstIfStatement* st);
    void emit(AstWhileStatement* st);
    void emit(AstCompoundStatement* st);
    void emitBranch(AstExp* exp, StringRef trueLabel);
    void emitBranchInverted(AstExp* exp, StringRef falseLabel);
    void emitBranch(AstExp* exp, StringRef label, bool invert);

    SkrExpRes emit(AstExp* exp, SkrVar* dst = nullptr);
    StringRef getFieldId(AstExp* exp);
    SkrExpRes emitBinary(AstBinaryExp* exp, SkrVar* dst = nullptr);
    SkrExpRes emitFunCall(AstFunCall* call, SkrVar* dst = nullptr);
    SkrExpRes emitStructInit(AstStructInit* it, SkrVar* dst = nullptr);
    SkrExpRes emitConstant(AstConstantExp* exp, SkrVar* dst);
    SkrExpRes emitAddrOf(AstAddrOf* exp, SkrVar* dst);
    SkrExpRes emitVar(AstVar* exp, SkrVar* dst);
    SkrExpRes emitAssignment(AstAssignment* exp);
    SkrExpRes emitCast(AstCast* exp, SkrVar* dst);
    SkrExpRes emitDot(AstDot* exp);

    SkrValue* emitAndConvert(AstExp* exp, SkrVar* dst = nullptr);

    SymbolType::Kind getTypeKind(SkrValue* value);

    SymbolType* getType(SkrValue* value);

    SymbolType* dereferenceType(SymbolType* t);
    SymbolType* dereferenceType(SkrValue* value);

    StringRef getStructTag(SymbolType* type);

    SkrVar* createVar(StringRef name, SymbolType* type);
    SkrVar* createVar(const char* name, SymbolType* type);
    SkrVar* createVar(StringRef name, const char* suffix, SymbolType* type);

    SkrFactory& skrf;
    SymbolTable& symbolTable;
    SymbolTypeFactory& typesf;
    TypeTable& typeTable;
    IdentifierGen& idGen;
    LabelGen& labelGen;
    std::vector<SkrInstruction*>& out;
    SkrVar* funcRetVal = nullptr;
    StringRef funName = StringRef::nullInstance();
    StringRef retLabel = StringRef::nullInstance();
};
