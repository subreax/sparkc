#pragma once
#include <map>
#include "../lexer/Token.h"
#include "../../common/IdentifierGen.h"
#include "../../common/Stack.h"
#include "except/DuplicateDeclarationException.h"
#include "except/UndeclaredVariableException.h"

class ScopeItem {
public:
    ScopeItem(StringRef first, const char* second) 
        : str(first)
        , value(second) {  }

    static ScopeItem beginning() {
        return ScopeItem(StringRef(nullptr, 0), nullptr);
    }

    const StringRef& getInitialId() const { return str; }
    const char* getUniqueId() const { return value; }

    bool isBeginning() const {
        return value == nullptr;
    }

private:
    StringRef str;
    const char* value;
};


class Scope {
public:
    Scope(IdentifierGen& idGen, LinearAllocator& stackMem) 
        : idGen(idGen), stack(stackMem) { openScope(); }

    const char* declare(const Token& token) {
        for (size_t i = 0; i < stack.getSize(); i++) {
            auto& it = stack.peek(i);
            if (it.isBeginning()) {
                break;
            }
            if (it.getInitialId() == token.value) {
                throw DuplicateDeclarationException(token);
            }
        }

        const char* uniqueId = idGen.unique(token.value);
        stack.push(ScopeItem(token.value, uniqueId));
        return uniqueId;
    }

    const char* resolve(const Token& token) {
        for (size_t i = 0; i < stack.getSize(); i++) {
            auto& it = stack.peek(i);
            if (it.getInitialId() == token.value) {
                return it.getUniqueId();
            }
        }
        throw UndeclaredVariableException(token);
    }

    void openScope() {
        stack.push(ScopeItem::beginning());
    }

    void closeScope() {
        while (stack.isNotEmpty()) {
            if (stack.peek().isBeginning()) {
                stack.pop();
                break;
            }
            stack.pop();
        }
    }

private:
    IdentifierGen& idGen;

    // [b] v v v [b] v v [b] v v v v v
    Stack<ScopeItem> stack;
};
