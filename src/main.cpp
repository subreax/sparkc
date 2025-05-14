#include <iostream>
#include <fstream>
#include <sstream>
#include "spark/frontend/Lexer.h"
using namespace std;


string readFile(const char* path) {
    ifstream fin(path);
    if (!fin) {
        return "";
    }

    ostringstream oss;
    oss << fin.rdbuf();
    return oss.str();
}

int main(int argc, char** argv) {
    if (argc == 1) {
        cout << "Specify source file to compile" << endl;
        return 1;
    }

    string source = readFile(argv[1]);
    Lexer lexer(source.c_str());
    char tokenValue[64];
    while (lexer.hasNext()) {
        auto token = lexer.next();
        token.value.copyTo(tokenValue, 64);
        printf("%-20s '%s' (%d:%d)\n", TokenKind_toString(token.kind), tokenValue, token.line, token.col);
    }

    return 0;
}
