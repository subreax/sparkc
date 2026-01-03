#pragma once
#include "sparkc/common/Error.h"
#include "sparkc/common/Stack.h"
#include "sparkc/common/StringRef.h"
#include "sparkc/symbol/SymbolTable.h"
#include "sparkc/type/TypeTable.h"

struct ScopeItem {
    enum class Kind {
        Var,
        Func,
        Struct,
        _Begin
    };

    ScopeItem(StringRef name, StringRef id, Kind kind)
        : name(name)
        , id(id)
        , kind(kind) { }

    StringRef name;
    StringRef id;
    Kind kind;
};

class Scope {
public:
    Scope(SymbolTable& table, TypeTable& typeTable, size_t memSize);

    void declareVar(StringRef name, StringRef id, SymbolType* type);

    void declareFunc(
        StringRef name,
        SymbolType* retType,
        const std::vector<SymbolType*>& params
    );

    void declareStruct(StringRef tag, const std::vector<StructField>& fields);
    const ScopeItem& get(StringRef name, ScopeItem::Kind kind) const;
    void open();
    void close();
    Allocator& getTypeAllocator() const;

private:
    void declareVarInScope(StringRef name, StringRef id);
    void declareFuncInScope(StringRef name);
    void declareStructInScope(StringRef tag);
    void declareInScope(StringRef name, StringRef id, ScopeItem::Kind kind);
    void init();

    static ScopeItem::Kind symbolKind2ScopeKind(SymbolType::Kind kind);

    LinearAllocator allocator;
    Stack<ScopeItem> stack;
    SymbolTable& symbolTable;
    TypeTable& typeTable;
};
