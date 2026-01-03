#include "ParserTestLoader.h"
#include <filesystem>
#include <fstream>
#include <sstream>

namespace ParserTestLoader {
namespace fs = std::filesystem;

void loadTestFile(const std::string& path, ParserTest& out);

void loadRecursively(
    const std::string& dirPath,
    std::function<void(const ParserTest&)> onTestLoaded
) {
    ParserTest test;
    auto dirIterator = fs::directory_iterator(dirPath);
    for (const auto& entry : dirIterator) {
        if (fs::is_regular_file(entry)) {
            loadTestFile(entry.path().string(), test);
            onTestLoaded(test);
        }
        else if (fs::is_directory(entry)) {
            loadRecursively(entry.path().string(), onTestLoaded);
        }
    }
}

std::string trim(const std::string& str) {
    size_t b = 0, e = str.length();
    while (b < e && std::isspace(str[b])) {
        b++;
    }

    while (e > b && std::isspace(str[e - 1])) {
        e--;
    }

    return str.substr(b, e - b);
}

void loadTestFile(const std::string& path, ParserTest& out) {
    std::ifstream fin(path);
    std::ostringstream sections[2];

    enum Section {
        S_SRC,
        S_EXPECTED
    };

    std::string line;
    Section section = S_SRC;

    while (getline(fin, line)) {
        if (line == "#src") {
            section = S_SRC;
        }
        else if (line == "#expect") {
            section = S_EXPECTED;
        }
        else {
            sections[section] << line << "\n";
        }
    }
    fin.close();

    out.path = path;
    out.src = trim(sections[S_SRC].str());
    out.expectedTree = trim(sections[S_EXPECTED].str());
}
}; // namespace ParserTestLoader
