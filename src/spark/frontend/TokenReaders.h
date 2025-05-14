#pragma once
#include "../common/StringRef.h"
#include "Token.h"
#include <string>
#include <cstring>

using TokenReader = StringRef(*)(const char*);

class TokenReaders {
public:
    static StringRef readIntKeyword(const char* src) {
        return readKeyword(src, "int");
    }

    static StringRef readReturnKeyword(const char* src) {
        return readKeyword(src, "return");
    }

    static StringRef readIdentifier(const char* src) {
        char first = src[0];
        if (!isalpha(first) && first != '_') {
            return StringRef::nullInstance();
        }

        int i = 1;
        while (src[i] != 0) {
            char c = src[i];
            if (isalnum(c) || c == '_') {
                i++;
            } else {
                break;
            }
        }
        return StringRef(src, i);
    }

    static StringRef readIntConstant(const char* src) {
        int i = 0;
        while (isdigit(src[i])) 
            i++;
        
        if (i > 0) {
            return StringRef(src, i);
        } else {
            return StringRef::nullInstance();
        }
    }

    static StringRef readOpenPar(const char* src) { return readChar(src, '('); }
    static StringRef readClosePar(const char* src) { return readChar(src, ')'); }
    static StringRef readOpenBrace(const char* src) { return readChar(src, '{'); }
    static StringRef readCloseBrace(const char* src) { return readChar(src, '}'); }
    static StringRef readSemicolon(const char* src) { return readChar(src, ';'); }
    static StringRef readPlus(const char* src) { return readChar(src, '+'); }
    static StringRef readHyphen(const char* src) { return readChar(src, '-'); }
    static StringRef readAsterisk(const char* src) { return readChar(src, '*'); }
    static StringRef readFwdSlash(const char* src) { return readChar(src, '/'); }
    static StringRef readPercent(const char* src) { return readChar(src, '%'); }
    static StringRef readEquals(const char* src) { return readChar(src, '='); }
    static StringRef readEOF(const char* src) { return readChar(src, '\0'); }

private:
    static StringRef readKeyword(const char* src, const char* value) {
        int valueLen = strlen(value);
        if (strcmp(src, value) == 0 && !isalpha(src[valueLen + 1])) {
            return StringRef(src, valueLen);
        } else {
            return StringRef::nullInstance();
        }
    }

    static StringRef readChar(const char* src, char c) {
        if (src[0] == c) {
            return StringRef(src, 1);
        } else {
            return StringRef::nullInstance();
        }
    }
};



