#include "Scope.h"

Scope::Scope(SymbolTable& table, TypeTable& typeTable, size_t memSize)
    : allocator("scope", memSize), stack(allocator), symbolTable(table),
      typeTable(typeTable) {
    init();
}

void Scope::declareVar(StringRef name, StringRef id, SymbolType* type) {
    declareVarInScope(name, id);
    symbolTable.declare(id, type);
}

void Scope::declareFunc(
    StringRef name,
    SymbolType* retType,
    const std::vector<SymbolType*>& params) {
    declareFuncInScope(name);
    symbolTable.declareFunc(name, retType, params);
}

void Scope::declareStruct(StringRef tag, const std::vector<StructField>& fields) {
    declareStructInScope(tag);
    typeTable.declare(tag, fields);
}

const ScopeItem& Scope::get(StringRef name, ScopeItem::Kind kind) const {
    for (size_t i = 0; i < stack.getSize(); i++) {
        const auto& it = stack.peek(i);
        if (it.name == name && it.kind == kind) {
            return it;
        }
    }
    sparkError("Scope", "Symbol was not declared: " + name.toString());
    throw "";
}

void Scope::open() {
    stack.push(ScopeItem(
        StringRef::nullInstance(),
        StringRef::nullInstance(),
        ScopeItem::Kind::_Begin));
}

void Scope::close() {
    while (stack.isNotEmpty()) {
        auto item = stack.peek();
        stack.pop();
        if (item.kind == ScopeItem::Kind::_Begin) {
            break;
        }
    }
}

Allocator& Scope::getTypeAllocator() const { return typeTable.getAllocator(); }


void Scope::declareVarInScope(StringRef name, StringRef id) {
    declareInScope(name, id, ScopeItem::Kind::Var);
}

void Scope::declareFuncInScope(StringRef name) {
    declareInScope(name, name, ScopeItem::Kind::Func);
}

void Scope::declareStructInScope(StringRef tag) {
    declareInScope(tag, tag, ScopeItem::Kind::Struct);
}

void Scope::declareInScope(StringRef name, StringRef id, ScopeItem::Kind kind) {
    for (size_t i = 0; i < stack.getSize(); i++) {
        auto& it = stack.peek(i);
        if (it.kind == ScopeItem::Kind::_Begin) {
            break;
        }

        if (it.name == name) {
            sparkError("Scope", "Duplicate declaration: " + name.toString());
        }
    }

    stack.push(ScopeItem(name, id, kind));
}

void Scope::init() {
    for (const auto& structEntry : typeTable) {
        declareStructInScope(structEntry.first);
    }

    for (const auto& entry : symbolTable) {
        declareFuncInScope(entry.first);
    }
}

ScopeItem::Kind Scope::symbolKind2ScopeKind(SymbolType::Kind kind) {
    switch (kind) {
    case SymbolType::Kind::Integer:
    case SymbolType::Kind::Float:
    case SymbolType::Kind::Pointer:
    case SymbolType::Kind::Structure: return ScopeItem::Kind::Var;

    case SymbolType::Kind::Function: return ScopeItem::Kind::Func;

    default:
        sparkError("Scope", "Unknown symbol kind: %d", kind);
        return ScopeItem::Kind::Var;
    }
}
