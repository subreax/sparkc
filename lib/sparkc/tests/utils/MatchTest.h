#pragma once
#include <string>

struct MatchTest {
    std::string path;
    std::string src;
    std::string expected;
    bool expectFailure;
};