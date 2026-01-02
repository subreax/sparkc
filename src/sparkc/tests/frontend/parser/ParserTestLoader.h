#pragma once
#include "ParserTest.h"
#include <functional>
#include <string>
#include <vector>

namespace ParserTestLoader {

void loadRecursively(
    const std::string& dirPath,
    std::function<void(const ParserTest&)> onTestLoaded);

}; // namespace ParserTestLoader