#include "scanner/scanner.h"

const std::vector<Token>& Scanner::tokens() {
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
