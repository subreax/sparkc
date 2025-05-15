#pragma once
#include <vector>
#include "../common/LinearAllocator.h"
#include "Lexer.h"
#include "ast/exp/everything.h"

template<typename T>
struct ParseResult {
    ParseResult(T value, bool isOk) : value(value), isOk(isOk) {  }

    T value;
    bool isOk;
};

class Parser {
public:
    Parser(Lexer& lexer, LinearAllocator& allocator) 
        : lexer(lexer)
        , allocator(allocator) 
    {
        takeToken();
    }

    ParseResult<AstExp*> parseExpression(int prevPrecedence = 0) {
        auto leftRes = parseFactor();
        if (!leftRes.isOk) { return leftRes; }

        AstExp* left = leftRes.value;
        int precedence = getPrecedence(current.kind);
        while (precedence >= prevPrecedence) {
            auto op = takeToken();
            auto rightRes = parseExpression(precedence + 1);
            if (!rightRes.isOk) { return rightRes; }
            left = allocator.create<AstBinaryExp>(left, AstBinaryExp::toBinaryOperator(op.kind), rightRes.value);
            precedence = getPrecedence(current.kind);
        }
        return ParseResult(left, true);
    }

    ParseResult<AstExp*> parseFactor() {
        if (current.kind == T_INT_CONSTANT) {
            auto token = takeToken();
            auto pr = parseInt(token.value);
            if (pr.isOk) {
                return ParseResult<AstExp*>(allocator.create<AstConstantExp>(pr.value), true);
            } else {
                return ParseResult<AstExp*>(nullptr, false);
            }
        }
        else if (current.kind == T_OPEN_PAR) {
            takeToken();
            auto exp = parseExpression();
            if (expect(T_CLOSE_PAR).kind == T_BAD) {
                return ParseResult<AstExp*>(nullptr, false);
            }
            return exp;
        } else {
            diag.emplace_back(std::string("Unexpected factor token: '") + current.value + "'");
            return ParseResult<AstExp*>(nullptr, false);
        }
    }

    const std::vector<std::string>& getDiagnostics() const {
        return diag;
    }

private:
    ParseResult<int32_t> parseInt(StringRef ref) {
        if (ref.getLength() > 10) {
            diag.emplace_back(std::string("Failed to parse int: ") + ref);
            return ParseResult(0, false); // max int length is 10
        }

        char buf[12];
        int len = ref.copyTo(buf, sizeof(buf));
        char* end;
        int32_t value = strtol(buf, &end, 10);
        if (*end != 0) {
            diag.emplace_back(std::string("Failed to parse int: ") + buf);
            return ParseResult(0, false);
        }
        return ParseResult(value, true);
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
            diag.emplace_back("Expected token " + TokenKind_toStdString(kind) + ", but found " + TokenKind_toStdString(current.kind));
            return Token();
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

    std::vector<std::string> diag;
    Token current;
    Lexer& lexer;
    LinearAllocator& allocator;
};