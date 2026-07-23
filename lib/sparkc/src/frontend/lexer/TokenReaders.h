#pragma once
#include "sparkc/common/StringRef.h"

namespace TokenReaders {
using Reader = StringRef (*)(const char*);

StringRef readVarKeyword(const char* src);
StringRef readIntKeyword(const char* src);
StringRef readFloatKeyword(const char* src);
StringRef readReturnKeyword(const char* src);
StringRef readIfKeyword(const char* src);
StringRef readElseKeyword(const char* src);
StringRef readWhileKeyword(const char* src);
StringRef readFunKeyword(const char* src);
StringRef readStructKeyword(const char* src);
StringRef readIdentifier(const char* src);
StringRef readIntConstant(const char* src);
StringRef readFloatConstant(const char* src);
StringRef readOpenPar(const char* src);
StringRef readClosePar(const char* src);
StringRef readOpenBrace(const char* src);
StringRef readCloseBrace(const char* src);
StringRef readColon(const char* src);
StringRef readSemicolon(const char* src);
StringRef readPlus(const char* src);
StringRef readHyphen(const char* src);
StringRef readAsterisk(const char* src);
StringRef readFwdSlash(const char* src);
StringRef readPercent(const char* src);
StringRef readEquals(const char* src);
StringRef readAmpAmp(const char* src);
StringRef readVBarVBar(const char* src);
StringRef readEqualsEquals(const char* src);
StringRef readNotEquals(const char* src);
StringRef readLessThan(const char* src);
StringRef readGreaterThan(const char* src);
StringRef readLessOrEq(const char* src);
StringRef readGreaterOrEq(const char* src);
StringRef readComma(const char* src);
StringRef readPeriod(const char* src);
StringRef readAmp(const char* src);
StringRef readEOF(const char* src);
}; // namespace TokenReaders
