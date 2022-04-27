#pragma once

#include <string>

typedef enum {
    TOKEN_ERR,
    TOKEN_NUM,
    TOKEN_QUOTE,
    TOKEN_ID,
    TOKEN_DEFINE,
    TOKEN_LET,
    TOKEN_LAMBDA,
    TOKEN_COND,
    TOKEN_LPARENT = '(',
    TOKEN_RPARENT = ')',
} TokenType;

class Token {
public:
    Token(std::string name);

    Token(): Token("") {}

    TokenType type() const {
        return type_;
    }

    const std::string& name() const {
        return name_;
    }

private:
    TokenType type_;
    std::string name_;

    bool isNum(char c);
};
