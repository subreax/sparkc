#include "Scope.h"

void Scope::declareVar(StringRef name, StringRef id) {
    declareInScope(name, id, ScopeItem::Kind::Var);
}

void Scope::declareFunc(StringRef name) {
    declareInScope(name, name, ScopeItem::Kind::Func);
}

void Scope::declareStruct(StringRef tag) {
    declareInScope(tag, tag, ScopeItem::Kind::Struct);
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
        ScopeItem::Kind::_Begin
    ));
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

ScopeItem::Kind Scope::symbolKind2ScopeKind(SymbolType::Kind kind) {
    switch (kind) {
    case SymbolType::Kind::Integer:
    case SymbolType::Kind::Float:
    case SymbolType::Kind::Pointer:
    case SymbolType::Kind::Structure:
        return ScopeItem::Kind::Var;

    case SymbolType::Kind::Function:
        return ScopeItem::Kind::Func;

    default:
        sparkError("Scope", "Unknown symbol kind: %d", kind);
        return ScopeItem::Kind::Var;
    }
}
