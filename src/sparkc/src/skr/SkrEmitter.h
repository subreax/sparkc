#pragma once
#include "sparkc/common/Error.h"
#include "sparkc/common/IdentifierGen.h"
#include "sparkc/common/LabelGen.h"
#include "sparkc/common/alloc/Allocator.h"
#include "sparkc/frontend/ast/everything.h"
#include "sparkc/skr/SkrFunction.h"
#include "sparkc/skr/instr/everything.h"
#include "sparkc/symbol/SymbolTable.h"
#include "sparkc/type/TypeTable.h"
#include "value/SkrExpRes.h"
#include <vector>

class SkrEmitter {
public:
    static SkrFunction* emit(
        AstFunction* func,
        Allocator& allocator,
        SymbolTable& symbolTable,
        TypeTable& typeTable,
        IdentifierGen& idGen,
        LabelGen& labelGen,
        std::vector<SkrInstruction*>& buf
    );

private:
    SkrEmitter(
        Allocator& allocator,
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
    void emitBranch(AstExp* exp, StringRef trueLabel);
    void emitInvertBranch(AstExp* exp, StringRef falseLabel);
    void emitBranch(AstExp* exp, StringRef label, bool invert);

    SkrExpRes emit(AstExp* exp, SkrVar* dst = nullptr);
    StringRef getFieldId(AstExp* exp);
    SkrValue* emitBinary(AstBinaryExp* exp, SkrVar* dst = nullptr);
    SkrValue* emitFunCall(AstFunCall* call, SkrVar* dst = nullptr);
    SkrValue* emitStructInit(AstStructInit* it, SkrVar* dst = nullptr);
    SkrValue* emitAndConvert(AstExp* exp, SkrVar* dst = nullptr);

    SymbolType::Kind getTypeKind(SkrValue* value);

    SymbolType* getType(SkrValue* value);

    SymbolType* dereferenceType(SymbolType* t);
    SymbolType* dereferenceType(SkrValue* value);

    StringRef getStructTag(SymbolType* type);

    void removeUselessJumpToRet();

    SkrVar* createVar(StringRef name, SymbolType* type);
    SkrVar* createVar(const char* name, SymbolType* type);
    SkrVar* createVar(StringRef name, const char* suffix, SymbolType* type);

    SkrConst* getSkrConst(Constant* c);
    SkrConst* getSkrIntConst(int32_t v);

    static SkrBinary::Operator binaryOpOf(AstBinaryExp::Operator astOp);
    static SkrBranch::Operator branchOpOf(AstBinaryExp::Operator astOp);
    static SkrBranch::Operator invertedBranchOpOf(AstBinaryExp::Operator astOp);

    static bool isLogicalBin(AstExp* exp);

    Allocator& allocator;
    SymbolTable& symbolTable;
    TypeTable& typeTable;
    IdentifierGen& idGen;
    LabelGen& labelGen;
    std::vector<SkrInstruction*>& out;
    SkrVar* funcRetVal = nullptr;
    StringRef funName = StringRef::nullInstance();
    StringRef retLabel = StringRef::nullInstance();
};
