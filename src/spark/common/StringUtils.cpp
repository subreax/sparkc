#include "StringUtils.h"

size_t StringUtils::copy(char* to, const char* from, size_t max) {
    if (max == 0) {
        return 0;
    }

    size_t i = 0;
    for ( ; *from && i < (max-1); i++) {
        to[i] = *from;
        from++;
    }
    to[i] = 0;
    return i;
}
