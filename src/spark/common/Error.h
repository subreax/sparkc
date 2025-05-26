#pragma once
#include <cstdio>
#include <cstdarg>
#include "SparkRuntimeException.h"

static void sparkError(const char* fileName, const char* fmt, ...) {
    char buf[192];
    int offset = snprintf(buf, sizeof(buf), "[%s] ", fileName);

    va_list args;
    va_start(args, fmt);
    vsnprintf(buf + offset, sizeof(buf) - offset, fmt, args);
    va_end(args);
    throw SparkRuntimeException(buf);
}
