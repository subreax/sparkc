#pragma once
#include "sparkc/common/StringRef.h"
#include <iostream>
#include <stack>

class Printer {
public:
    Printer(std::ostream& out) : out(out) {}

    void beginObject(const char* name) {
        out << name;
        openScope("(");
    }

    void endObject() { closeScope(")"); }

    void field(const char* name) {
        nextElement();
        out << name << ": ";
    }

    void value(const char* stringValue) { out << "'" << stringValue << "'"; }

    void value(int val) { out << val; }

    void value(float val) { out << val; }

    void beginArray() { openScope("["); }

    void arrItem() { nextElement(); }

    void endArray() { closeScope("]"); }

    void field(const char* name, const char* val) {
        field(name);
        value(val);
    }

    void field(const char* name, const StringRef& val) {
        field(name);
        value(val.toString().c_str());
    }

    void field(const char* name, const std::string& s) {
        field(name);
        value(s.c_str());
    }

    void field(const char* name, int val) {
        field(name);
        value(val);
    }

    void field(const char* name, float val) {
        field(name);
        value(val);
    }

private:
    void printIndent() {
        for (int i = 0; i < indent; i++) {
            out << ' ';
        }
    }

    void openScope(const char* opener) {
        out << opener;
        indent += INDENT;

        elemScopes.push(elements);
        elements = 0;
    }

    void closeScope(const char* closer) {
        indent -= INDENT;
        if (elements != 0) {
            out << "\n";
            printIndent();
        }
        out << closer;

        elements = elemScopes.top();
        elemScopes.pop();
    }

    void nextElement() {
        if (elements != 0) {
            out << ",";
        }
        out << "\n";
        printIndent();
        elements++;
    }

    std::stack<int> elemScopes;
    int elements = 0;

    std::ostream& out;
    int indent = 0;

    static constexpr int INDENT = 2;
};
