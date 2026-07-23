#include "TokenReaders.h"
#include <cstring>

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

static StringRef readKeyword(const char* src, const char* keyword) {
    int keywordLen = strlen(keyword);
    if (startsWith(src, keyword) && !(isalnum(src[keywordLen]))) {
        return StringRef(src, keywordLen);
    }
    else {
        return StringRef::nullInstance();
    }
}

static StringRef readChar(const char* src, char c) {
    if (src[0] == c) {
        return StringRef(src, 1);
    }
    else {
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

StringRef TokenReaders::readVarKeyword(const char* src) { return readKeyword(src, "var"); }
StringRef TokenReaders::readIntKeyword(const char* src) { return readKeyword(src, "int"); }
StringRef TokenReaders::readFloatKeyword(const char* src) { return readKeyword(src, "float"); }
StringRef TokenReaders::readReturnKeyword(const char* src) { return readKeyword(src, "return"); }
StringRef TokenReaders::readIfKeyword(const char* src) { return readKeyword(src, "if"); }
StringRef TokenReaders::readElseKeyword(const char* src) { return readKeyword(src, "else"); }
StringRef TokenReaders::readWhileKeyword(const char* src) { return readKeyword(src, "while"); }
StringRef TokenReaders::readFunKeyword(const char* src) { return readKeyword(src, "fun"); }
StringRef TokenReaders::readStructKeyword(const char* src) { return readKeyword(src, "struct"); }

StringRef TokenReaders::readIdentifier(const char* src) {
    char first = src[0];
    if (!isalpha(first) && first != '_') {
        return StringRef::nullInstance();
    }

    int i = 1;
    while (src[i] != 0) {
        char c = src[i];
        if (isalnum(c) || c == '_') {
            i++;
        }
        else {
            break;
        }
    }
    return StringRef(src, i);
}

StringRef TokenReaders::readIntConstant(const char* src) {
    int i = 0;
    while (isdigit(src[i]))
        i++;

    if (i > 0) {
        return StringRef(src, i);
    }
    else {
        return StringRef::nullInstance();
    }
}

StringRef TokenReaders::readFloatConstant(const char* src) {
    int i = 0;
    while (isdigit(src[i])) {
        i++;
    }

    if (src[i] == '.') {
        i++;
    }
    else {
        return StringRef::nullInstance(); // because it is integer or not a number
    }

    int oldI = i;
    while (isdigit(src[i])) {
        i++;
    }
    if (i == oldI) {
        return StringRef::nullInstance(); // after dot should be at least 1 digit
    }

    if (src[i] == 'f' || src[i] == 'F') {
        i++;
    }

    return StringRef(src, i);
}

StringRef TokenReaders::readOpenPar(const char* src) { return readChar(src, '('); }
StringRef TokenReaders::readClosePar(const char* src) { return readChar(src, ')'); }
StringRef TokenReaders::readOpenBrace(const char* src) { return readChar(src, '{'); }
StringRef TokenReaders::readCloseBrace(const char* src) { return readChar(src, '}'); }
StringRef TokenReaders::readColon(const char* src) { return readChar(src, ':'); }
StringRef TokenReaders::readSemicolon(const char* src) { return readChar(src, ';'); }
StringRef TokenReaders::readPlus(const char* src) { return readChar(src, '+'); }
StringRef TokenReaders::readHyphen(const char* src) { return readChar(src, '-'); }
StringRef TokenReaders::readAsterisk(const char* src) { return readChar(src, '*'); }
StringRef TokenReaders::readFwdSlash(const char* src) { return readChar(src, '/'); }
StringRef TokenReaders::readPercent(const char* src) { return readChar(src, '%'); }
StringRef TokenReaders::readEquals(const char* src) { return readChar(src, '='); }
StringRef TokenReaders::readAmpAmp(const char* src) { return readString(src, "&&"); }
StringRef TokenReaders::readVBarVBar(const char* src) { return readString(src, "||"); }
StringRef TokenReaders::readEqualsEquals(const char* src) { return readString(src, "=="); }
StringRef TokenReaders::readNotEquals(const char* src) { return readString(src, "!="); }
StringRef TokenReaders::readLessThan(const char* src) { return readChar(src, '<'); }
StringRef TokenReaders::readGreaterThan(const char* src) { return readChar(src, '>'); }
StringRef TokenReaders::readLessOrEq(const char* src) { return readString(src, "<="); }
StringRef TokenReaders::readGreaterOrEq(const char* src) { return readString(src, ">="); }
StringRef TokenReaders::readComma(const char* src) { return readChar(src, ','); }
StringRef TokenReaders::readPeriod(const char* src) { return readChar(src, '.'); }
StringRef TokenReaders::readAmp(const char* src) { return readChar(src, '&'); }
StringRef TokenReaders::readEOF(const char* src) { return readChar(src, '\0'); }
