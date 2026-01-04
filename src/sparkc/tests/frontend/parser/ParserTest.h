#pragma once
#include <string>

struct ParserTest {
    std::string path;
    std::string src;

    std::string expectedTree;
    bool expectFailure;
};
