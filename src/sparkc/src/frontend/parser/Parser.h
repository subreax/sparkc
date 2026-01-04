#pragma once
#include "../lexer/Lexer.h"
#include "sparkc/common/alloc/Allocator.h"
#include "sparkc/frontend/ast/everything.h"
#include "sparkc/frontend/parser/except/everything.h"
#include <vector>

class Parser {
public:
    Parser(Lexer& lexer, Allocator& allocator, Allocator& sharedAlloc);

    AstProgram* parseProgram();

    bool hasNext() const;

    void declareType(StringRef type);
    bool isTypeDeclared(StringRef type);

private:
    template <typename T, typename... Args>
    inline T* newNode(Args... args) {
        return allocator.create<T>(args...);
    }

    AstStruct* tryParseStruct();
    AstFunction* parseFunction();
    AstFunParam* parseFunParam();
    AstBlock* parseBlock();
    AstBlockItem* parseBlockItem();
    AstDeclaration* tryParseDeclaration();

    AstStatement* parseStatement();
    AstIfStatement* parseIfStatement();
    AstStatement* parseWhileStatement();

    AstExp* parseExpression(int prevPrecedence = 0);
    AstExp* parseFactor();
    void parseFunArgs(std::vector<AstExp*>& outArgs);
    SymbolType* parseType();
    SymbolType* tryParseType();

    static int32_t parseInt(const Token& token);
    static float parseFloat(const Token& token);

    Token takeToken();
    Token expect(TokenKind kind);

    Token current;
    Lexer& lexer;
    Allocator& allocator;
    Allocator& sharedAllocator;
    std::vector<StringRef> types;
};