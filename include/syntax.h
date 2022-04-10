#include <istream>
#include <iostream>
#include <vector>

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

    bool isNum(char c) {
        return c >= '0' && c <= '9';
    }
};

class TokenStream {
public:
    TokenStream(std::istream& stream): stream_{stream} {}
    std::string& next();

private:
    std::istream& stream_;
    std::string buffer_;
    std::string token_;
    bool read_{true};

    bool is_parentheses(char c) {
        return c == TOKEN_LPARENT || c == TOKEN_RPARENT;
    }
};

class Syntax {
public:
    Syntax(std::istream& stream): tokenStream_{stream} {
        read();
    }

    std::vector<Token> tokens() const;
    void read();
    void clear();

private:
    TokenStream tokenStream_;
    std::vector<Token> tokens_;
};