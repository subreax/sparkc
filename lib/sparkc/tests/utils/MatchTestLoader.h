#pragma once
#include <functional>
#include "MatchTest.h"

class MatchTestLoader {
public:
    static void loadRecursively(
        const std::string& basePath,
        std::function<void(const MatchTest&)> onTestLoaded
    );
};