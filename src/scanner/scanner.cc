#include "scanner/scanner.h"

std::vector<Token> Scanner::tokens() const {
    return tokens_;
}

void Scanner::read() {
    std::string name;
    while ((name = tokenStream_.next()) != "") {
        tokens_.emplace_back(name);
    }
}

void Scanner::clear() {
    tokens_.clear();
}
