#pragma once

#include <string>

typedef enum {
    TOKEN_ERR,
    TOKEN_NUM,
    TOKEN_QUOTE,
    TOKEN_ID,
    TOKEN_DEFINE,
    TOKEN_LET,
    TOKEN_LPARENT = '(',
    TOKEN_RPARENT = ')',
} TokenType;

class Token {
public:
    Token(std::string name);

    TokenType type() {
        return type_;
    }

    std::string& name() {
        return name_;
    }

private:
    TokenType type_;
    std::string name_;

    bool isNum(char c);
};
