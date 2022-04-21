#include "scanner/token.h"

Token::Token(std::string name): name_{name} {
    if (name.size() == 0) {
        type_ = TOKEN_ERR;
        return;
    }

    if (isNum(name[0]) || name[0] == '-') {
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

bool Token::isNum(char c) {
    return c >= '0' && c <= '9';
}
