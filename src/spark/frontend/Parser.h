#pragma once
#include <vector>
#include "../common/LinearAllocator.h"
#include "Lexer.h"
#include "ast/exp/everything.h"
#include "ast/AstProgram.h"

#include "exceptions/UnexpectedTokenExcepion.h"
#include "exceptions/ParseConstException.h"
#include "exceptions/WrongExprException.h"
#include "exceptions/WrongStatementException.h"

#define PARSER_RET_IF_FAIL(res, type) if ((res).failed()) { return failure<type>(); }

template<typename T>
struct ParseResult {
    ParseResult(T* value, bool isOk = true) : value(value), isOk(isOk && value != nullptr) {  }

    inline bool failed() const { return !isOk; }

    T* value;
    bool isOk;
};

class Parser {
public:
    Parser(Lexer& lexer, LinearAllocator& allocator, LinearAllocator& idAllocator)
        : lexer(lexer)
        , objAlloc(allocator)
        , idAlloc(idAllocator)
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
        auto* st = parseStatement();
        expect(T_CLOSE_BRACE);

        std::vector<AstStatement*> statements;
        statements.emplace_back(st);
        return objAlloc.create<AstFunction>(funName, statements);
    }

    AstStatement* parseStatement() {
        if (current.kind == T_RETURN_KEYWORD) {
            takeToken();
            AstExp* exp = parseExpression();
            expect(T_SEMICOLON);
            return objAlloc.create<AstReturnStatement>(exp);
        }
        else {
            throw WrongStatementException(current);
        }
    }

    AstExp* parseExpression(int prevPrecedence = 0) {
        AstExp* left = parseFactor();
        int precedence = getPrecedence(current.kind);
        while (precedence >= prevPrecedence) {
            auto op = takeToken();
            AstExp* right = parseExpression(precedence + 1);
            left = objAlloc.create<AstBinaryExp>(left, AstBinaryExp::toBinaryOperator(op.kind), right);
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
        } else {
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

    template<typename T>
    inline ParseResult<T> failure() {
        return ParseResult<T>(nullptr, false);
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
        
        default:
            return -1;
        }
    }

    Token current;
    Lexer& lexer;
    LinearAllocator& objAlloc;
    LinearAllocator& idAlloc;
};