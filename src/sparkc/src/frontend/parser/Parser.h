#pragma once
#include "../lexer/Lexer.h"
#include "sparkc/common/alloc/Allocator.h"
#include "sparkc/frontend/ast/everything.h"
#include "sparkc/frontend/parser/except/everything.h"
#include "sparkc/type/TypeTable.h"
#include <vector>

class Parser {
public:
    Parser(
        Lexer& lexer,
        Allocator& allocator,
        TypeTable& typeTable,
        std::vector<StringRef>& types);

    AstProgram* parseProgram();

    bool hasNext() const;

private:
    AstStruct* tryParseStruct();
    AstFunction* parseFunction();
    AstFunParam* parseFunParam();
    AstBlock* parseBlock();
    AstBlockItem* parseBlockItem();
    AstDeclaration* tryParseDeclaration();
    AstStatement* parseStatement();
    AstExp* parseExpression(int prevPrecedence = 0);
    AstExp* parseFactor();
    void parseFunArgs(std::vector<AstExp*>& outArgs);
    SymbolType* parseType();
    SymbolType* tryParseType();

    static int32_t parseInt(const Token& token);
    static float parseFloat(const Token& token);
    static int getPrecedence(TokenKind tk);

    Token takeToken();
    Token expect(TokenKind kind);

    void regType(StringRef type);
    bool isTypeExist(StringRef type);

    Token current;
    Lexer& lexer;
    Allocator& allocator;
    TypeTable& typeTable;
    std::vector<StringRef>& types;
};