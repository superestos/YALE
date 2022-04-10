#include "scanner/scanner.h"

std::vector<Token> Syntax::tokens() const {
    return tokens_;
}

void Syntax::read() {
    std::string name;
    while ((name = tokenStream_.next()) != "") {
        tokens_.emplace_back(name);
    }
}

void Syntax::clear() {
    tokens_.clear();
}
