#pragma once
#include "../../common/Stack.h"
#include "../../common/StringRef.h"
#include "../../common/Error.h"
#include "../../symbol/SymbolTable.h"
#include "../../type/TypeTable.h"

#include "../ast/AstStruct.h"

struct ScopeItem {
    enum class Kind {
        Var, Func, Struct, _Begin
    };

    ScopeItem(StringRef name, StringRef id, Kind kind)
        : name(name), id(id) , kind(kind) {  }

    StringRef name;
    StringRef id;
    Kind kind;
};

class Scope {
public:
    Scope(SymbolTable& table, TypeTable& typeTable, size_t memSize) 
        : allocator("scope", memSize)
        , stack(allocator)
        , symbolTable(table)
        , typeTable(typeTable) {  }

    void declareVar(StringRef name, StringRef id, SymbolType* type) {
        declareInScope(name, id, ScopeItem::Kind::Var);
        symbolTable.declare(id, type);
    }

    void declareFunc(StringRef name, SymbolType* retType, const std::vector<SymbolType*>& params) {
        declareInScope(name, name, ScopeItem::Kind::Func);
        symbolTable.declareFunc(name, retType, params);
    }

    void declareStruct(StringRef tag, const std::vector<StructField>& fields) {
        declareInScope(tag, tag, ScopeItem::Kind::Struct);
        typeTable.declare(tag, fields);
    }

    const ScopeItem& get(StringRef name, ScopeItem::Kind kind) const {
        for (size_t i = 0; i < stack.getSize(); i++) {
            const auto& it = stack.peek(i);
            if (it.name == name && it.kind == kind) {
                return it;
            }
        }
        sparkError("Scope", "Symbol was not declared: " + name.toString());
        throw "";
    }

    void open() {
        stack.push(ScopeItem(StringRef::nullInstance(), StringRef::nullInstance(), ScopeItem::Kind::_Begin));
    }

    void close() {
        while (stack.isNotEmpty()) {
            auto item = stack.peek();
            stack.pop();
            if (item.kind == ScopeItem::Kind::_Begin) {
                break;
            }
        }
    }

    Allocator& getTypeAllocator() const {
        return typeTable.getAllocator();
    }

private:
    void declareInScope(StringRef name, StringRef id, ScopeItem::Kind kind) {
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

    LinearAllocator allocator;
    Stack<ScopeItem> stack;
    SymbolTable& symbolTable;
    TypeTable& typeTable;
};
