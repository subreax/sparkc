#include "Constant.h"

int32_t Constant::intValue() const {
    return ((IntConstant*) this)->val;
}

float Constant::floatValue() const {
    return ((FloatConstant*) this)->val;
}