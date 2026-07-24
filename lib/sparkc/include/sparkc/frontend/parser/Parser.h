#pragma once
#include <vector>
#include "sparkc/common/alloc/Allocator.h"
#include "sparkc/frontend/lexer/Lexer.h"
#include "sparkc/frontend/ast/AstFactory.h"
#include "sparkc/frontend/parser/except/everything.h"

class Parser {
public:
    Parser(Lexer& lexer, AstFactory& astFactory, Allocator& sharedAlloc);

    AstProgram* parseProgram();

    bool hasNext() const;

private:
    AstStruct* tryParseStruct();
    AstStructField* parseStructField();
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
    AstFactory& astf;
    Allocator& sharedAllocator;
    std::vector<StringRef> types;
};