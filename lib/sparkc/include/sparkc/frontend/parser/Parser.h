#pragma once
#include <vector>
#include "sparkc/frontend/lexer/Lexer.h"
#include "sparkc/frontend/ast/AstFactory.h"
#include "sparkc/frontend/parser/except/everything.h"
#include "sparkc/symbol/SymbolTypeFactory.h"

class Parser {
public:
    Parser(Lexer& lexer, AstFactory& astFactory, SymbolTypeFactory& symbolTypeFactory);

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

    static int32_t parseInt(const Token& token);
    static float parseFloat(const Token& token);

    Token takeToken();
    Token expect(TokenKind kind);

    Token current;
    Lexer& lexer;
    AstFactory& astf;
    SymbolTypeFactory& typesf;
};
