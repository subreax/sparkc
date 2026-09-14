#pragma once

class AstProgItem {
public:
    enum class Kind {
        Variable,
        Function,
        Struct
    };

    AstProgItem(Kind kind)
        : kind(kind) { }

    const Kind kind;
};
