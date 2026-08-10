#pragma once
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <filesystem>

class FileUtils {
public:
    static bool readFile(const std::string& path, std::string& out) {
        std::ifstream fin(path);
        if (!fin) {
            return false;
        }

        std::ostringstream oss;
        oss << fin.rdbuf();
        out = oss.str();
        return true;
    }

    static std::string getFileName(const std::string& path) {
        return path.substr(getLastSlashPos(path) + 1);
    }

    static std::string changeExtension(const std::string& path, const std::string& newExt) {
        int dot = path.rfind('.');
        if (dot == std::string::npos) {
            return path + "." + newExt;
        }

        return path.substr(0, dot) + "." + newExt;
    }

    static bool createDirectories(const std::string& path) {
        auto dir = std::filesystem::path(path).parent_path();

        if (dir.empty()) {
            return true;
        }

        std::error_code ec;
        std::filesystem::create_directories(dir, ec);

        if (ec) {
            std::cerr << "Failed to create directories: " << ec.message() << std::endl;
            return false;
        }

        return true;
    }

private:
    static int getLastSlashPos(const std::string& path) {
        size_t slash = path.rfind('/');
        if (slash != std::string::npos) {
            return slash;
        }

        slash = path.rfind('\\');
        if (slash != std::string::npos) {
            return slash;
        }

        return -1;
    }
};
