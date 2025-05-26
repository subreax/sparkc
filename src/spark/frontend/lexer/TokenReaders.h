#pragma once
#include "../../common/StringRef.h"
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
    static StringRef readAmpAmp(const char* src) { return readString(src, "&&"); }
    static StringRef readVBarVBar(const char* src) { return readString(src, "||"); }
    static StringRef readComma(const char* src) { return readChar(src, ','); }
    static StringRef readEOF(const char* src) { return readChar(src, '\0'); }

private:
    static StringRef readKeyword(const char* src, const char* keyword) {
        int keywordLen = strlen(keyword);
        if (startsWith(src, keyword) && !(isalnum(src[keywordLen]))) {
            return StringRef(src, keywordLen);
        } else {
            return StringRef::nullInstance();
        }
    }

    static bool startsWith(const char* str, const char* beginning) {
        size_t i = 0;
        while (str[i] && beginning[i]) {
            if (str[i] != beginning[i]) {
                return false;
            }
            i++;
        }
        if (beginning[i] == '\0') {
            return true;
        }
        return false;
    }

    static StringRef readChar(const char* src, char c) {
        if (src[0] == c) {
            return StringRef(src, 1);
        } else {
            return StringRef::nullInstance();
        }
    }

    static StringRef readString(const char* src, const char* what) {
        size_t len = 0;
        while (src[len] && what[len]) {
            if (src[len] != what[len]) {
                return StringRef::nullInstance();
            }
            len++;
        }
        
        if (what[len] != '\0') {
            return StringRef::nullInstance();
        }
        return StringRef(src, len);
    }
};



