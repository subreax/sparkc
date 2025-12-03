#include "sparkc/frontend/lexer/Lexer.h"
#include "TokenReaders.h"


struct TokenReaderWithKind {
    TokenKind kind;
    TokenReaders::Reader read;
};

static constexpr TokenReaderWithKind Lexer_readers[] = {
    { T_INT_KEYWORD, TokenReaders::readIntKeyword },
    { T_FLOAT_KEYWORD, TokenReaders::readFloatKeyword },
    { T_RETURN_KEYWORD, TokenReaders::readReturnKeyword },
    { T_IF_KEYWORD, TokenReaders::readIfKeyword },
    { T_ELSE_KEYWORD, TokenReaders::readElseKeyword },
    { T_WHILE_KEYWORD, TokenReaders::readWhileKeyword },
    { T_STRUCT_KEYWORD, TokenReaders::readStructKeyword },
    { T_IDENTIFIER, TokenReaders::readIdentifier },
    { T_FLOAT_CONSTANT, TokenReaders::readFloatConstant },
    { T_INT_CONSTANT, TokenReaders::readIntConstant },
    { T_OPEN_PAR, TokenReaders::readOpenPar },
    { T_CLOSE_PAR, TokenReaders::readClosePar },
    { T_OPEN_BRACE, TokenReaders::readOpenBrace },
    { T_CLOSE_BRACE, TokenReaders::readCloseBrace },
    { T_SEMICOLON, TokenReaders::readSemicolon },
    { T_PLUS, TokenReaders::readPlus },
    { T_HYPHEN, TokenReaders::readHyphen },
    { T_ASTERISK, TokenReaders::readAsterisk },
    { T_FWD_SLASH, TokenReaders::readFwdSlash },
    { T_PERCENT, TokenReaders::readPercent },
    { T_PERCENT, TokenReaders::readPercent },
    { T_EQUALS_EQUALS, TokenReaders::readEqualsEquals, },
    { T_NOT_EQUALS, TokenReaders::readNotEquals, },
    { T_EQUALS, TokenReaders::readEquals },
    { T_AMP_AMP, TokenReaders::readAmpAmp },
    { T_AMP, TokenReaders::readAmp },
    { T_VBAR_VBAR, TokenReaders::readVBarVBar },
    { T_LESS_OR_EQ, TokenReaders::readLessOrEq },
    { T_LESS_THAN, TokenReaders::readLessThan },
    { T_GREATER_OR_EQ, TokenReaders::readGreaterOrEq },
    { T_GREATER_THAN, TokenReaders::readGreaterThan },
    { T_COMMA, TokenReaders::readComma },
    { T_PERIOD, TokenReaders::readPeriod },
    { T_EOF, TokenReaders::readEOF }
};

static constexpr int Lexer_readersCount = sizeof(Lexer_readers) / sizeof(TokenReaderWithKind);


Lexer::Lexer(const char* src) : src(src) { }

Token Lexer::next() {
    skipWhitespaces();
    StringRef value = StringRef::nullInstance();
    TokenKind kind = T_BAD;
    for (int i = 0; i < Lexer_readersCount; i++) {
        const auto& reader = Lexer_readers[i];
        value = reader.read(src + pos);
        if (value.isNotNull()) {
            kind = reader.kind;
            break;
        }
    }

    if (value.isNotNull()) {
        Token token(kind, value, line, column);
        auto len = value.getLength();
        pos += len;
        column += len;
        return token;
    } else {
        Token token(kind, StringRef(src, 1), line, column);
        pos += 1;
        column += 1;
        return token;
    }
}

bool Lexer::hasNext() {
    return src[pos] != 0;
}


void Lexer::skipWhitespaces() {
    while (isspace(src[pos])) {
        if (src[pos] == '\n') {
            line++;
            column = -1;
        }
        pos++;
        column++;
    }
}
