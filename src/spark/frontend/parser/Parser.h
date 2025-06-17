#pragma once
#include <vector>
#include "../../common/alloc/Allocator.h"
#include "../../common/IdentifierGen.h"
#include "../lexer/Lexer.h"
#include "../ast/everything.h"
#include "Scope.h"
#include "except/everything.h"

class Parser {
public:
    Parser(
        Lexer& lexer, 
        Allocator& allocator, 
        IdentifierGen& idGen,
        Scope& scope
    )
        : lexer(lexer)
        , allocator(allocator)
        , idGen(idGen)
        , scope(scope)
    {
        takeToken();
    }

    AstProgram* parseProgram() {
        std::vector<AstFunction*> functions;
        while (hasNext()) {
            auto func = parseFunction();
            functions.emplace_back(func);
        }
        auto arrFunctions = BoundArray<AstFunction*>::fromVector(functions, allocator);
        auto* prog = allocator.create<AstProgram>(arrFunctions);
        return prog;
    }

    AstFunction* parseFunction() {
        auto* retType = parseType();
        Token idToken = expect(T_IDENTIFIER);
        const char* funName = idGen.copy(idToken.value);

        scope.openScope();

        expect(T_OPEN_PAR);
        std::vector<AstFunParam*> params;
        while (current.kind != T_CLOSE_PAR) {
            params.emplace_back(parseFunParam());
            if (current.kind == T_COMMA) {
                takeToken();
            }
        }
        takeToken();

        AstBlock* block = parseBlock();

        scope.closeScope();

        auto paramsBa = BoundArray<AstFunParam*>::fromVector(params, allocator);
        return allocator.create<AstFunction>(funName, retType, paramsBa, block);
    }

    AstFunParam* parseFunParam() {
        auto* type = parseType();
        auto id = scope.declare(expect(T_IDENTIFIER));
        return allocator.create<AstFunParam>(id, type);
    }

    AstBlock* parseBlock() {
        expect(T_OPEN_BRACE);
        scope.openScope();

        std::vector<AstBlockItem*> items;
        while (current.kind != T_CLOSE_BRACE) {
            items.emplace_back(parseBlockItem());
        }
        
        scope.closeScope();
        expect(T_CLOSE_BRACE);

        auto itemsBa = BoundArray<AstBlockItem*>::fromVector(items, allocator);
        return allocator.create<AstBlock>(itemsBa);
    }

    AstBlockItem* parseBlockItem() {
        auto* decl = tryParseDeclaration();
        if (decl != nullptr) {
            return allocator.create<AstDeclBlockItem>(decl);
        }
        return allocator.create<AstStatementBlockItem>(parseStatement());
    }

    AstDeclaration* tryParseDeclaration() {
        SymbolType* type = tryParseType();
        if (type != nullptr) {
            const char* varName = scope.declare(expect(T_IDENTIFIER));
            AstExp* initializer = nullptr;
            if (current.kind == T_EQUALS) {
                takeToken();
                initializer = parseExpression();
            }
            expect(T_SEMICOLON);
            return allocator.create<AstVarDeclaration>(varName, type, initializer);
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
            return allocator.create<AstReturnStatement>(exp);
        }
        else if (current.kind == T_IF_KEYWORD) {
            takeToken();
            expect(T_OPEN_PAR);
            AstExp* cond = parseExpression();
            expect(T_CLOSE_PAR);
            AstStatement* ifTrue = parseStatement();
            AstStatement* ifFalse = nullptr;
            if (current.kind == T_ELSE_KEYWORD) {
                takeToken();
                ifFalse = parseStatement();
            }
            return allocator.create<AstIfStatement>(cond, ifTrue, ifFalse);
        }
        else if (current.kind == T_WHILE_KEYWORD) {
            takeToken();
            expect(T_OPEN_PAR);
            auto* cond = parseExpression();
            expect(T_CLOSE_PAR);
            auto* st = parseStatement();
            return allocator.create<AstWhileStatement>(cond, st);
        }
        else if (current.kind == T_OPEN_BRACE) {
            AstBlock* block = parseBlock();
            return allocator.create<AstCompoundStatement>(block);
        }
        else {
            AstExp* exp = parseExpression();
            expect(TokenKind::T_SEMICOLON);
            return allocator.create<AstExpressionStatement>(exp);
        }
    }

    AstExp* parseExpression(int prevPrecedence = 0) {
        AstExp* left = parseFactor();
        int precedence = getPrecedence(current.kind);
        while (precedence >= prevPrecedence) {
            auto op = takeToken();
            if (op.kind != T_EQUALS) {
                AstExp* right = parseExpression(precedence + 1);
                left = allocator.create<AstBinaryExp>(left, AstBinaryExp::toBinaryOperator(op.kind), right);
            } else {
                AstExp* right = parseExpression(precedence);
                left = allocator.create<AstAssignment>(left, right);
            }
            precedence = getPrecedence(current.kind);
        }
        return left;
    }

    AstExp* parseFactor() {
        if (current.kind == T_INT_CONSTANT) {
            auto token = takeToken();
            int32_t value = parseInt(token);
            auto* c = allocator.create<IntConstant>(value);
            return allocator.create<AstConstantExp>(c);
        }
        else if (current.kind == T_FLOAT_CONSTANT) {
            auto token = takeToken();
            auto* c = allocator.create<FloatConstant>(parseFloat(token));
            return allocator.create<AstConstantExp>(c);
        }
        else if (current.kind == T_OPEN_PAR) {
            takeToken();
            auto exp = parseExpression();
            expect(T_CLOSE_PAR);
            return exp;
        }
        else if (current.kind == T_IDENTIFIER) {
            auto nameToken = takeToken();
            if (current.kind == T_OPEN_PAR) {
                takeToken();
                std::vector<AstExp*> args;
                parseFunArgs(args);
                expect(T_CLOSE_PAR);

                const char* funName = idGen.copy(nameToken.value);
                auto argsBA = BoundArray<AstExp*>::fromVector(args, allocator);
                return allocator.create<AstFunCall>(funName, argsBA);
            }
            else {
                return allocator.create<AstVar>(scope.resolve(nameToken));
            }
        }
        else {
            throw WrongExprException(current);
        }
    }

    void parseFunArgs(std::vector<AstExp*>& outArgs) {
        while (current.kind != T_CLOSE_PAR) {
            outArgs.emplace_back(parseExpression());
            if (current.kind == T_COMMA) {
                takeToken();
            }
        }
    }

    bool hasNext() const { return current.kind != T_EOF; }

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

    float parseFloat(const Token& token) {
        if (token.value.getLength() > 12) {
            throw ParseConstException(token);
        }

        char buf[16];
        int len = token.value.copyTo(buf, sizeof(buf));

        char* end;
        float value = strtof(buf, &end);
        if (*end != 0) {
            throw ParseConstException(token);
        }
        return value;
    }

    SymbolType* parseType() {
        auto* type = tryParseType();
        if (type == nullptr) {
            throw UnknownTypeException(current);
        }
        return type;
    }

    SymbolType* tryParseType() {
        if (current.kind == T_INT_KEYWORD) {
            takeToken();
            return SymbolIntType::getInstance();
        }
        else if (current.kind == T_FLOAT_KEYWORD) {
            takeToken();
            return SymbolFloatType::getInstance();
        }
        else {
            return nullptr;
        }
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
        case T_PERCENT:
            return 50;
        
        case T_PLUS:
        case T_HYPHEN:
            return 40;
        
        case T_GREATER_THAN:
        case T_LESS_THAN:
        case T_GREATER_OR_EQ:
        case T_LESS_OR_EQ:
            return 35;

        case T_EQUALS_EQUALS:
        case T_NOT_EQUALS:
            return 30;

        case T_AMP_AMP:
            return 10;

        case T_VBAR_VBAR:
            return 5;

        case T_EQUALS:
            return 1;
        
        default:
            return -1;
        }
    }

    Token current;
    Lexer& lexer;
    Allocator& allocator;
    IdentifierGen& idGen;
    Scope& scope;
};