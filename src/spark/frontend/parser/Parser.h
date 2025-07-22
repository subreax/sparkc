#pragma once
#include <vector>
#include "../../common/alloc/Allocator.h"
#include "../../common/IdentifierGen.h"
#include "../lexer/Lexer.h"
#include "../ast/everything.h"
#include "except/everything.h"

class Parser {
public:
    Parser(
        Lexer& lexer, 
        Allocator& allocator, 
        Allocator& typeAlloc
    )
        : lexer(lexer)
        , allocator(allocator)
        , typeAlloc(typeAlloc)
    {
        takeToken();
    }

    AstProgram* parseProgram() {
        std::vector<AstProgItem*> items;
        while (hasNext()) {
            auto* st = tryParseStruct();
            if (st != nullptr) {
                items.emplace_back(st);
                continue;
            }

            items.emplace_back(parseFunction());
        }
        auto arrFunctions = BoundArray<AstProgItem*>::fromVector(items, allocator);
        return allocator.create<AstProgram>(arrFunctions);
    }

    bool hasNext() const { return current.kind != T_EOF; }

private:
    AstStruct* tryParseStruct() {
        if (current.kind == T_STRUCT_KEYWORD) {
            takeToken();
            StringRef tag = expect(T_IDENTIFIER).value;
            regType(tag);
            expect(T_OPEN_PAR);
            std::vector<AstStructField*> fields;
            while (hasNext() && current.kind != T_CLOSE_PAR) {
                auto* type = parseType();
                auto name = expect(T_IDENTIFIER).value;
                fields.emplace_back(allocator.create<AstStructField>(type, name));
                if (current.kind == T_COMMA) {
                    takeToken();
                }
            }
            expect(T_CLOSE_PAR);
            expect(T_SEMICOLON);

            return allocator.create<AstStruct>(tag, BoundArray<AstStructField*>::fromVector(fields, allocator));
        }
        return nullptr;
    }

    AstFunction* parseFunction() {
        auto* retType = parseType();
        Token idToken = expect(T_IDENTIFIER);
        StringRef funName = idToken.value;

        expect(T_OPEN_PAR);
        std::vector<AstFunParam*> params;
        while (hasNext() && current.kind != T_CLOSE_PAR) {
            params.emplace_back(parseFunParam());
            if (current.kind == T_COMMA) {
                takeToken();
            }
        }
        takeToken();

        AstBlock* block = parseBlock();

        auto paramsBa = BoundArray<AstFunParam*>::fromVector(params, allocator);
        return allocator.create<AstFunction>(funName, retType, paramsBa, block);
    }

    AstFunParam* parseFunParam() {
        auto* type = parseType();
        auto id = expect(T_IDENTIFIER).value;
        return allocator.create<AstFunParam>(id, type);
    }

    AstBlock* parseBlock() {
        expect(T_OPEN_BRACE);
        std::vector<AstBlockItem*> items;
        while (hasNext() && current.kind != T_CLOSE_BRACE) {
            items.emplace_back(parseBlockItem());
        }
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
            StringRef varName = expect(T_IDENTIFIER).value;
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
            if (op.kind == T_EQUALS) {
                AstExp* right = parseExpression(precedence);
                left = allocator.create<AstAssignment>(left, right);
            }
            else if (op.kind == T_PERIOD) {
                AstExp* right = parseExpression(precedence + 1);
                left = allocator.create<AstDot>(left, right);
            }
            else {
                AstExp* right = parseExpression(precedence + 1);
                left = allocator.create<AstBinaryExp>(left, AstBinaryExp::toBinaryOperator(op.kind), right);
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
            auto id = takeToken().value;
            if (current.kind == T_OPEN_PAR) {
                takeToken();
                std::vector<AstExp*> args;
                parseFunArgs(args);
                expect(T_CLOSE_PAR);

                auto argsBA = BoundArray<AstExp*>::fromVector(args, allocator);
                if (isTypeExist(id)) {
                    return allocator.create<AstStructInit>(id, argsBA);
                } else {
                    return allocator.create<AstFunCall>(id, argsBA);
                }
            }
            else {
                return allocator.create<AstVar>(id);
            }
        }
        else {
            throw WrongExprException(current);
        }
    }

    void parseFunArgs(std::vector<AstExp*>& outArgs) {
        while (hasNext() && current.kind != T_CLOSE_PAR) {
            outArgs.emplace_back(parseExpression());
            if (current.kind == T_COMMA) {
                takeToken();
            }
        }
    }

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
        SymbolType* type;
        if (current.kind == T_INT_KEYWORD) {
            takeToken();
            type = SymbolIntType::getInstance();
        }
        else if (current.kind == T_FLOAT_KEYWORD) {
            takeToken();
            type = SymbolFloatType::getInstance();
        }
        else if (current.kind == T_IDENTIFIER && isTypeExist(current.value)) {
            auto token = takeToken();
            type = typeAlloc.create<SymbolStructureType>(token.value);
        }
        else {
            return nullptr;
        }

        return type;

        /* if (current.kind == T_AMP) {
            takeToken();
            return typeAlloc.create<SymbolPointerType>(type);
        } else {
            return type;
        } */
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
        case T_PERIOD:
            return 100;

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

    void regType(StringRef type) {
        if (isTypeExist(type)) {
            sparkError("Parser", "Type %d is already declared", type); // todo: replace
        }
        types.emplace_back(type);
    }

    bool isTypeExist(StringRef type) {
        for (auto t : types) {
            if (t == type) {
                return true;
            }
        }
        return false;
    }

    std::vector<StringRef> types;

    Token current;
    Lexer& lexer;
    Allocator& allocator;
    Allocator& typeAlloc;
};