#pragma once
#include "Token.h"
#include "TokenReaders.h"


class Lexer {
public:
    Lexer(const char* src) : src(src) { }

    Token next() {
        skipWhitespaces();
        StringRef value = StringRef::nullInstance();
        TokenKind kind = T_BAD;
        for (int i = 0; i < readersCount; i++) {
            const auto& reader = readers[i];
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

    bool hasNext() {
        return src[pos] != 0;
    }

private:
    struct TokenReaderWithKind {
        TokenKind kind;
        TokenReader read;
    };

    void skipWhitespaces() {
        while (isspace(src[pos])) {
            if (src[pos] == '\n') {
                line++;
                column = -1;
            }
            pos++;
            column++;
        }
    }

    static constexpr TokenReaderWithKind readers[] = {
        { T_INT_KEYWORD, TokenReaders::readIntKeyword },
        { T_RETURN_KEYWORD, TokenReaders::readReturnKeyword },
        { T_IDENTIFIER, TokenReaders::readIdentifier },
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
        { T_EQUALS, TokenReaders::readEquals },
        { T_AMP_AMP, TokenReaders::readAmpAmp },
        { T_VBAR_VBAR, TokenReaders::readVBarVBar },
        { T_EOF, TokenReaders::readEOF }
    };

    static constexpr int readersCount = sizeof(readers) / sizeof(TokenReaderWithKind);

    const char* src;
    int pos = 0;
    int line = 0;
    int column = 0;
};
