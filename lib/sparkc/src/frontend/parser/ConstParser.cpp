#include "ConstParser.h"
#include <cstring>

static int compareInts(StringRef a, StringRef b) {
    if (a.getLength() > b.getLength()) {
        return 1;
    }
    else if (a.getLength() < b.getLength()) {
        return -1;
    }

    for (int i = a.getLength(); i >= 0; i--) {
        char ca = a[i];
        char cb = b[i];
        if (ca > cb) {
            return 1;
        }
        else if (ca < cb) {
            return -1;
        }
    }
    return 0;
}

bool ConstParser::parseInt(StringRef strValue, int32_t& out) {
    if (strValue.getLength() >= 12 || compareInts(strValue, StringRef::cstr("2147483647")) > 0) {
        return false;
    }

    char buf[16];
    int len = strValue.copyTo(buf, sizeof(buf));
    char* end;
    int32_t value = strtol(buf, &end, 10);
    if (*end != 0) {
        return false;
    }
    out = value;
    return true;
}

bool ConstParser::parseFloat(StringRef strValue, float& out) {
    if (strValue.getLength() == 0 || strValue.getLength() >= 16) {
        return false;
    }

    char buf[20];
    int len = strValue.copyTo(buf, sizeof(buf));
    if (tolower(buf[len - 1]) == 'f') {
        len -= 1;
        buf[len] = '\0';
    }

    char* end;
    float value = strtof(buf, &end);
    if (*end != 0) {
        return false;
    }
    out = value;
    return true;
}