#include "syntax.h"

Token::Token(std::string name): name_{name} {
    if (name.size() == 0) {
        type_ = TOKEN_ERR;
        return;
    }

    if (isNum(name[0])) {
        for (size_t i = 1; i < name.size(); i++) {
            if (!isNum(name[i])) {
                type_ = TOKEN_ERR;
                return;
            }
        }

        type_ = TOKEN_NUM;
        return;
    }

    if (name[0] == '\'') {
        type_ = TOKEN_QUOTE;
        return;
    }

    if (name == "define") {
        type_ = TOKEN_DEFINE;
        return;
    }

    if (name == "let") {
        type_ = TOKEN_LET;
        return;
    }

    if (name == "(") {
        type_ = TOKEN_LPARENT;
        return;
    }

    if (name == ")") {
        type_ = TOKEN_RPARENT;
        return;
    }

    type_ = TOKEN_ID;
}

std::string& TokenStream::next() {
    if (read_) {
        stream_ >> buffer_;
    }

    size_t cut = buffer_.size();
    for (size_t i = 0; i < buffer_.size(); i++) {
        if (is_parentheses(buffer_[i])) {
            cut = std::max(1UL, i);
            break;
        }
    }

    token_ = buffer_.substr(0, cut);
    buffer_ = buffer_.substr(cut);
    read_ = buffer_.empty();
    
    return token_;
}

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
