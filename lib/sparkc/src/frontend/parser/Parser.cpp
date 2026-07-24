#include "sparkc/frontend/parser/Parser.h"
#include "sparkc/frontend/parser/Precedence.h"
#include "sparkc/frontend/parser/ConstParser.h"

Parser::Parser(Lexer& lexer, AstFactory& astFactory, SymbolTypeFactory& symbolTypeFactory)
    : lexer(lexer)
    , astf(astFactory)
    , typesf(symbolTypeFactory) {
    takeToken();
}

AstProgram* Parser::parseProgram() {
    std::vector<AstProgItem*> items;
    while (hasNext()) {
        auto* st = tryParseStruct();
        if (st != nullptr) {
            items.emplace_back(st);
            continue;
        }

        items.emplace_back(parseFunction());
    }

    return astf.program(items);
}

bool Parser::hasNext() const { return current.kind != T_EOF; }

AstStruct* Parser::tryParseStruct() {
    if (current.kind == T_STRUCT_KEYWORD) {
        takeToken();
        StringRef tag = expect(T_IDENTIFIER).value;
        expect(T_OPEN_PAR);
        std::vector<AstStructField*> fields;
        bool expectNextField = true;
        while (expectNextField && current.kind != T_CLOSE_PAR) {
            fields.emplace_back(parseStructField());
            if (current.kind == T_COMMA) {
                takeToken();
                expectNextField = true;
            }
            else {
                expectNextField = false;
            }
        }
        expect(T_CLOSE_PAR);
        expect(T_SEMICOLON);
        return astf.struct_(tag, fields);
    }
    return nullptr;
}

AstStructField* Parser::parseStructField() {
    auto name = expect(T_IDENTIFIER).value;
    expect(T_COLON);
    auto* type = parseType();
    return astf.structField(name, type);
}

AstFunction* Parser::parseFunction() {
    expect(T_FUN_KEYWORD);
    Token idToken = expect(T_IDENTIFIER);
    StringRef funName = idToken.value;

    expect(T_OPEN_PAR);
    std::vector<AstFunParam*> params;
    bool expectNextArg = true;
    while (expectNextArg && current.kind != T_CLOSE_PAR) {
        params.emplace_back(parseFunParam());
        if (current.kind == T_COMMA) {
            takeToken();
            expectNextArg = true;
        }
        else {
            expectNextArg = false;
        }
    }
    expect(T_CLOSE_PAR);

    expect(T_COLON);
    auto* retType = parseType();

    AstBlock* block = parseBlock();

    return astf.function(funName, retType, params, block);
}

AstFunParam* Parser::parseFunParam() {
    auto id = expect(T_IDENTIFIER).value;
    expect(T_COLON);
    auto* type = parseType();
    return astf.functionParam(id, type);
}

AstBlock* Parser::parseBlock() {
    expect(T_OPEN_BRACE);
    std::vector<AstBlockItem*> items;
    while (hasNext() && current.kind != T_CLOSE_BRACE) {
        items.emplace_back(parseBlockItem());
    }
    expect(T_CLOSE_BRACE);
    return astf.block(items);
}

AstBlockItem* Parser::parseBlockItem() {
    auto* decl = tryParseDeclaration();
    if (decl != nullptr) {
        return astf.declBlockItem(decl);
    }
    return astf.statementBlockItem(parseStatement());
}

AstDeclaration* Parser::tryParseDeclaration() {
    if (current.kind == T_VAR_KEYWORD) {
        takeToken();
        StringRef varName = expect(T_IDENTIFIER).value;
        SymbolType* type = nullptr;
        AstExp* initializer = nullptr;

        if (current.kind == T_COLON) {
            expect(T_COLON);
            type = parseType();
        }

        if (current.kind == T_EQUALS) {
            takeToken();
            initializer = parseExpression();
        }
        expect(T_SEMICOLON);
        return astf.varDeclaration(varName, type, initializer);
    }

    return nullptr;
}

AstStatement* Parser::parseStatement() {
    switch (current.kind) {
    case T_RETURN_KEYWORD: {
        takeToken();
        AstExp* exp = parseExpression();
        expect(T_SEMICOLON);
        return astf.returnStatement(exp);
    }

    case T_IF_KEYWORD:
        return parseIfStatement();

    case T_WHILE_KEYWORD:
        return parseWhileStatement();

    case T_OPEN_BRACE:
        return astf.compoundStatement(parseBlock());

    default: {
        AstExp* exp = parseExpression();
        expect(T_SEMICOLON);
        return astf.expressionStatement(exp);
    }
    }
}

AstIfStatement* Parser::parseIfStatement() {
    expect(T_IF_KEYWORD);
    expect(T_OPEN_PAR);
    AstExp* cond = parseExpression();
    expect(T_CLOSE_PAR);

    AstStatement* ifTrue = parseStatement();
    AstStatement* ifFalse = nullptr;

    if (current.kind == T_ELSE_KEYWORD) {
        takeToken();
        ifFalse = parseStatement();
    }

    return astf.ifStatement(cond, ifTrue, ifFalse);
}

AstStatement* Parser::parseWhileStatement() {
    expect(T_WHILE_KEYWORD);
    expect(T_OPEN_PAR);
    AstExp* cond = parseExpression();
    expect(T_CLOSE_PAR);

    AstStatement* body = parseStatement();
    return astf.whileStatement(cond, body);
}

AstExp* Parser::parseExpression(int prevPrecedence) {
    AstExp* left = parseFactor();
    int precedence = Precedence::get(current.kind);
    while (precedence >= prevPrecedence) {
        auto op = takeToken();
        if (op.kind == T_EQUALS) {
            AstExp* right = parseExpression(precedence);
            left = astf.assignment(left, right);
        }
        else if (op.kind == T_PERIOD) {
            AstExp* right = parseExpression(precedence + 1);
            left = astf.dot(left, right);
        }
        else {
            AstExp* right = parseExpression(precedence + 1);
            left = astf.binaryExp(left, op.kind, right);
        }
        precedence = Precedence::get(current.kind);
    }
    return left;
}

AstExp* Parser::parseFactor() {
    switch (current.kind) {
    case T_INT_CONSTANT: {
        auto token = takeToken();
        int32_t value = parseInt(token);
        return astf.intConstantExp(value);
    }

    case T_FLOAT_CONSTANT: {
        auto token = takeToken();
        return astf.floatConstantExp(parseFloat(token));
    }

    case T_OPEN_PAR: {
        takeToken();
        auto exp = parseExpression();
        expect(T_CLOSE_PAR);
        return exp;
    }

    case T_IDENTIFIER: {
        auto id = takeToken().value;

        if (current.kind == T_OPEN_PAR) {
            takeToken();
            std::vector<AstExp*> args;
            parseFunArgs(args);
            expect(T_CLOSE_PAR);

            return astf.funCall(id, args);
        }
        else {
            return astf.var(id);
        }
    }

    default: throw WrongExprException(current);
    }
}

void Parser::parseFunArgs(std::vector<AstExp*>& outArgs) {
    while (hasNext() && current.kind != T_CLOSE_PAR) {
        outArgs.emplace_back(parseExpression());
        if (current.kind == T_COMMA) {
            takeToken();
        }
    }
}

int32_t Parser::parseInt(const Token& token) {
    int32_t parsed;
    if (!ConstParser::parseInt(token.value, parsed)) {
        throw ParseConstException(token);
    }
    return parsed;
}

float Parser::parseFloat(const Token& token) {
    float parsed;
    if (!ConstParser::parseFloat(token.value, parsed)) {
        throw ParseConstException(token);
    }
    return parsed;
}

SymbolType* Parser::parseType() {
    SymbolType* type = nullptr;
    switch (current.kind) {
    case T_INT_KEYWORD:
        takeToken();
        type = typesf.int_();
        break;

    case T_FLOAT_KEYWORD:
        takeToken();
        type = typesf.float_();
        break;

    case T_IDENTIFIER:
        type = typesf.structure(takeToken().value);
        break;

    default:
        throw UnknownTypeException(current);
    }

    return type;
}

Token Parser::takeToken() {
    auto current0 = current;
    current = lexer.next();
    return current0;
}

Token Parser::expect(TokenKind kind) {
    if (current.kind == kind) {
        return takeToken();
    }
    else {
        throw UnexpectedTokenException(kind, current);
    }
}
