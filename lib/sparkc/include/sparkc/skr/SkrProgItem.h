#pragma once

class SkrProgItem {
public:
    enum class Kind {
        Var,
        Function
    };

    const Kind kind;

protected:
    SkrProgItem(Kind kind)
        : kind(kind) { }
};
