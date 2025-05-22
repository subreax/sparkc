#pragma once
#include <vector>
#include "../common/LinearAllocator.h"
#include "Lexer.h"
#include "ast/AstProgram.h"
#include "ast/AstBlockItem.h"
#include "ast/declaration/AstVarDeclaration.h"
#include "ast/statement/everything.h"
#include "ast/exp/everything.h"
#include "Scope.h"

#include "exceptions/UnexpectedTokenExcepion.h"
#include "exceptions/ParseConstException.h"
#include "exceptions/WrongExprException.h"
#include "exceptions/WrongStatementException.h"

class Parser {
public:
    Parser(
        Lexer& lexer, 
        LinearAllocator& allocator, 
        LinearAllocator& idAllocator,
        Scope& scope
    )
        : lexer(lexer)
        , objAlloc(allocator)
        , idAlloc(idAllocator)
        , scope(scope)
    {
        takeToken();
    }

    AstProgram* parseProgram() {
        auto* prog = objAlloc.create<AstProgram>();
        auto func = parseFunction();
        prog->functions.emplace_back(func);
        return prog;
    }

    AstFunction* parseFunction() {
        expect(T_INT_KEYWORD);
        Token idToken = expect(T_IDENTIFIER);

        auto idLen = idToken.value.getLength() + 1;
        char* funName = (char*) idAlloc.allocate(idLen);
        idToken.value.copyTo(funName, idLen);

        expect(T_OPEN_PAR);
        expect(T_CLOSE_PAR);
        expect(T_OPEN_BRACE);

        std::vector<AstBlockItem*> items;
        while (current.kind != T_CLOSE_BRACE) {
            items.emplace_back(parseBlockItem());
        }
        expect(T_CLOSE_BRACE);

        return objAlloc.create<AstFunction>(funName, items);
    }

    AstBlockItem* parseBlockItem() {
        auto* decl = tryParseDeclaration();
        if (decl != nullptr) {
            return objAlloc.create<AstDeclBlockItem>(decl);
        }
        return objAlloc.create<AstStatementBlockItem>(parseStatement());
    }

    AstDeclaration* tryParseDeclaration() {
        if (current.kind == T_INT_KEYWORD) {
            takeToken();
            const char* varName = scope.declare(takeToken());
            AstExp* initializer = nullptr;
            if (current.kind == T_EQUALS) {
                takeToken();
                initializer = parseExpression();
            }
            expect(T_SEMICOLON);
            return objAlloc.create<AstVarDeclaration>(varName, initializer);
        }
        else {
            return nullptr;
        }
    }

    AstStatement* parseStatement() {
        if (current.kind == T_RETURN_KEYWORD) {
            takeToken();
            AstExp* exp = parseExpression();
            expect(T_SEMICOLON);
            return objAlloc.create<AstReturnStatement>(exp);
        }
        else {
            AstExp* exp = parseExpression();
            expect(TokenKind::T_SEMICOLON);
            return objAlloc.create<AstExpressionStatement>(exp);
        }
    }

    AstExp* parseExpression(int prevPrecedence = 0) {
        AstExp* left = parseFactor();
        int precedence = getPrecedence(current.kind);
        while (precedence >= prevPrecedence) {
            auto op = takeToken();
            if (op.kind != T_EQUALS) {
                AstExp* right = parseExpression(precedence + 1);
                left = objAlloc.create<AstBinaryExp>(left, AstBinaryExp::toBinaryOperator(op.kind), right);
            } else {
                AstExp* right = parseExpression(precedence);
                left = objAlloc.create<AstAssignment>(left, right);
            }
            precedence = getPrecedence(current.kind);
        }
        return left;
    }

    AstExp* parseFactor() {
        if (current.kind == T_INT_CONSTANT) {
            auto token = takeToken();
            int32_t value = parseInt(token);
            return objAlloc.create<AstConstantExp>(value);
        }
        else if (current.kind == T_OPEN_PAR) {
            takeToken();
            auto exp = parseExpression();
            expect(T_CLOSE_PAR);
            return exp;
        } 
        else if (current.kind == T_IDENTIFIER) {
            const char* name = scope.resolve(takeToken());
            return objAlloc.create<AstVar>(name);
        }
        else {
            throw WrongExprException(current);
        }
    }

private:
    int32_t parseInt(const Token& token) {
        if (token.value.getLength() > 12) {
            throw ParseConstException(token);
        }

        char buf[16];
        int len = token.value.copyTo(buf, sizeof(buf));
        char* end;
        int32_t value = strtol(buf, &end, 10);
        if (*end != 0) {
            throw ParseConstException(token);
        }
        return value;
    }

    Token takeToken() {
        auto current0 = current;
        current = lexer.next();
        return current0;
    }

    Token expect(TokenKind kind) {
        if (current.kind == kind) {
            return takeToken();
        } else {
            throw UnexpectedTokenException(kind, current);
        }
    }

    int getPrecedence(TokenKind tk) {
        switch (tk)
        {
        case T_ASTERISK:
        case T_FWD_SLASH:
            return 50;
        
        case T_PLUS:
        case T_HYPHEN:
        case T_PERCENT:
            return 40;

        case T_EQUALS:
            return 1;
        
        default:
            return -1;
        }
    }

    Token current;
    Lexer& lexer;
    LinearAllocator& objAlloc;
    LinearAllocator& idAlloc;
    Scope& scope;
};