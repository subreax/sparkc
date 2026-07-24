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
    void declareVar(StringRef name, StringRef id);
    void declareFunc(StringRef name);
    void declareStruct(StringRef tag);

    const ScopeItem& get(StringRef name, ScopeItem::Kind kind) const;
    bool isDeclared(StringRef name, ScopeItem::Kind kind) const;

    void open();
    void close();

private:
    static ScopeItem::Kind symbolKind2ScopeKind(SymbolType::Kind kind);

    void declareInScope(StringRef name, StringRef id, ScopeItem::Kind kind);
    int findItemOrNeg1(StringRef name, ScopeItem::Kind kind) const;

    Stack<ScopeItem> stack;
};
