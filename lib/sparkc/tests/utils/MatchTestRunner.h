#pragma once
#include <catch2/catch_test_macros.hpp>
#include <functional>
#include "MatchTestLoader.h"
#include "StringUtils.h"

class MatchTestRunner {
public:
    static void runAllTests(const std::string& baseDir, std::function<std::string(const std::string& src)> onRun) {
        MatchTestLoader::loadRecursively(baseDir, [&](const MatchTest& test) {
            runTest(test, onRun);
        });
    }

private:
    static void runTest(const MatchTest& test, std::function<std::string(const std::string& src)> onRun) {
        INFO("Test: " << test.path);
        bool expectValue = !test.expectFailure;
        if (expectValue) {
            try {
                auto actual = StringUtils::trim(onRun(test.src));
                REQUIRE(actual == test.expected);
            } catch (std::exception& ex) {
                FAIL("Failed with exception:\n  " << ex.what());
            }
        }
        else {
            REQUIRE_THROWS(onRun(test.src));
        }
    }
};