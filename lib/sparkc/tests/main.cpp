#include <catch2/catch_session.hpp>
#include "TestOptions.h"

TestOptions testOptions;

int main(int argc, char* argv[]) {
    Catch::Session session;

    using namespace Catch::Clara;

    auto cli = session.cli()
        | Opt(testOptions.parserMatchTestsPath, "path")
            ["--parser-tests"]("Path to parser match tests folder");

    session.cli(cli);

    return session.applyCommandLine(argc, argv) || session.run();
}