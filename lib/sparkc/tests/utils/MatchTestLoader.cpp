#include "MatchTestLoader.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include "StringUtils.h"

namespace fs = std::filesystem;

static MatchTest loadTestFile(const std::string& path);

void MatchTestLoader::loadRecursively(
    const std::string& basePath,
    std::function<void(const MatchTest&)> onTestLoaded
) {
    auto dirIterator = fs::directory_iterator(basePath);
    for (const auto& entry : dirIterator) {
        if (fs::is_regular_file(entry)) {
            MatchTest test = loadTestFile(entry.path().string());
            onTestLoaded(test);
        }
        else if (fs::is_directory(entry)) {
            loadRecursively(entry.path().string(), onTestLoaded);
        }
    }
}

MatchTest loadTestFile(const std::string& path) {
    std::ifstream fin(path);
    std::ostringstream sections[2];

    enum Section {
        S_SRC,
        S_EXPECTED
    };

    std::string line;
    Section section = S_SRC;
    bool expectFailure = false;

    while (getline(fin, line)) {
        if (line == "#src") {
            section = S_SRC;
        }
        else if (line == "#expect") {
            section = S_EXPECTED;
        }
        else if (line == "#expect_failure") {
            expectFailure = true;
        }
        else {
            sections[section] << line << "\n";
        }
    }
    fin.close();

    MatchTest out;
    out.path = path;
    out.src = StringUtils::trim(sections[S_SRC].str());
    out.expected = StringUtils::trim(sections[S_EXPECTED].str());
    out.expectFailure = expectFailure;
    return out;
}